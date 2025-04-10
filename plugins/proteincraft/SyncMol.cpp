/**
 * This file is part of ProteinCraft (https://github.com/nanovis/ProteinCraft)
 *
 * Authors: Roden Luo
 *
 * ProteinCraft builds on top of Tulip.
 *
 */
#include <QEventLoop>
#include <tulip/TlpQtTools.h>
#include <tulip/PropertyAlgorithm.h>
#include <tulip/BooleanProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/Graph.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

using namespace tlp;

static const char *paramHelp[] = {
    // base url
    "ChimeraX REST URL (default port 45145)",
    // base path
    "Base path for structure files"};

class SyncMol : public tlp::Algorithm {

public:
    PLUGININFORMATION(
        "SyncMol", "Roden Luo", "2025/04/09",
        "Sync molecule with ChimeraX",
        "1.2", "ProteinCraft")

    SyncMol(const tlp::PluginContext *context)
        : Algorithm(context) {
        addInParameter<std::string>("base url", paramHelp[0], "http://127.0.0.1:45145/run");
        addInParameter<std::string>("base path", paramHelp[1], "/home/luod/ProteinCraft/run/4_PD-L1/outs_AF2ig_reindex");
    }

    std::string category() const override {
        return "ProteinCraft";
    }

    bool check(std::string &errMsg) override {
        return true;
    }

    bool run() override {
      
        std::string baseUrl;
        std::string basePath;

        if (dataSet != nullptr) {
            dataSet->get("base url", baseUrl);
            dataSet->get("base path", basePath);
        }

        QJsonObject syncData;

        // Find the required graphs
        Graph *AF2ig_graph = nullptr;
        Graph *Tetris_graph = nullptr;
        Graph *UMAP_graph = nullptr;

        for (Graph *g : tlp::getRootGraphs()) {
            std::string name = g->getName();
            if (name.find("AF2ig") == 0) {
                AF2ig_graph = g;
            } else if (name.find("Tetris") == 0) {
                Tetris_graph = g;
            } else if (name.find("UMAP") == 0) {
                UMAP_graph = g;
            }
        }

        // Process AF2ig graph
        if (AF2ig_graph) {
            BooleanProperty *viewSelection = AF2ig_graph->getProperty<BooleanProperty>("viewSelection");
            StringProperty *description = AF2ig_graph->getProperty<StringProperty>("description");

            for (node n : AF2ig_graph->getNodes()) {
                if (viewSelection->getNodeValue(n)) {
                    std::string filename = description->getNodeValue(n);
                    std::string fullPath = basePath + "/" + filename + ".pdb";
                    std::string nodeId = std::to_string(n.id);

                    QJsonObject nodeData;
                    nodeData["id"] = QString::fromStdString(nodeId);
                    nodeData["name"] = QString::fromStdString(filename);
                    nodeData["display"] = true;

                    syncData[QString::fromStdString(fullPath)] = nodeData;
                }
            }
        }

        // // Process Tetris graph
        // if (Tetris_graph) {
        //     BooleanProperty *viewSelection = Tetris_graph->getProperty<BooleanProperty>("viewSelection");
        //     StringProperty *description = Tetris_graph->getProperty<StringProperty>("description");
        //
        //     for (node n : Tetris_graph->getNodes()) {
        //         if (viewSelection->getNodeValue(n)) {
        //             std::string filename = description->getNodeValue(n);
        //             std::string fullPath = basePath + "/" + filename + ".pdb";
        //             std::string nodeId = std::to_string(n.id);
        //
        //             QJsonObject nodeData;
        //             nodeData["id"] = QString::fromStdString(nodeId);
        //             nodeData["name"] = QString::fromStdString(filename);
        //             nodeData["display"] = true;
        //
        //             syncData[QString::fromStdString(fullPath)] = nodeData;
        //         }
        //     }
        // }

        // Convert sync data to JSON string
        QJsonDocument doc(syncData);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        QString command = QString("proteincraft sync_mol jsonString '%1'").arg(jsonString);

        // Send request to ChimeraX
        QNetworkAccessManager manager;
        QUrl url(QString::fromStdString(baseUrl));
        QUrlQuery query;
        query.addQueryItem("command", command);
        url.setQuery(query);

        QNetworkRequest request(url);
        QNetworkReply *reply = manager.get(request);

        // Wait for the reply
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() != QNetworkReply::NoError) {
            if (pluginProgress) {
                pluginProgress->setError(QString("Failed to connect to ChimeraX: %1").arg(reply->errorString()).toStdString());
            }
            return false;
        }

        return true;
    }
};

PLUGIN(SyncMol)
