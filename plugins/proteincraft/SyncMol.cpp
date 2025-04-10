/**
*
 * This file is part of ProteinCraft (https://github.com/nanovis/ProteinCraft)
 *
 * Authors: Roden Luo
 *
 * ProteinCraft builds on top of Tulip.
 *
 */
#include <tulip/TlpQtTools.h>
#include <tulip/PropertyAlgorithm.h>
#include <tulip/BooleanProperty.h>
#include <tulip/StringProperty.h>

using namespace tlp;

class SyncMol : public tlp::StringAlgorithm {

public:
    PLUGININFORMATION(
        "SyncMol", "Roden Luo", "2025/04/09",
        "Sync molecule with ChimeraX",
        "1.2", "ProteinCraft")

    SyncMol(const tlp::PluginContext *context)
        // set second parameter of the constructor below to true because
        // result needs to be an inout parameter
        // in order to preserve the original values of non targeted elements
        // i.e if "nodes" == true and "edges==false", the values of edges must be preserved
        : StringAlgorithm(context, true){

    }

    std::string category() const override {
        // return ALGORITHM_CATEGORY;
        return "ProteinCraft";
    }

    bool check(std::string &errMsg) override {
        return true;
    }

    bool run() override {


#ifndef NDEBUG

        qDebug() << "SyncMol";

#endif // NDEBUG

        return true;
    }
};

PLUGIN(SyncMol)
