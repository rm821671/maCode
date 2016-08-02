
#include "rooFitHeader.h"

void rooFitter(){
    
    //gROOT->Reset();
    //gSystem->Load("pluginTreeWriterTreeWriterAuto.so");
    
    string filepath, dropbox, ss, dataset;
    
    // filepath and dropbox path changes, depending on system
    SystemPath(filepath, dropbox);
    
    ss = "root_selectorFiles/";
    
    string filenames[] = 
        {   ///no b2b, higher resolution in Z mass spectrum MC
            "myTestSelector_NEW_tnp_forPlots_1460453023.root",  // 0
            /// back to back 0.8 pi to 1.2 pi MC
            "myTestSelector_NEW_tnp_forPlots_1460459442.root",   // 1
            /// back to back 0.9 pi to 1.1 pi MC
            "myTestSelector_NEW_tnp_forPlots_1460464275.root", //2
            /// complete data
            "myTestSelector_NEW_data_tnp_forPlots_1460633565.root", //3
            
            // complete data with back to back (>0.9 Pi):
            "myTestSelector_events_SingleElectron__1461591971.root", //4
            
            // muon bkg no trigger
            "myTestSelector_muon_bkg_estimation_noTriggermatching_1462183761.root",//5
            
            // signal template from monte carlo
            "myTestSelector_signalTemplates_1462878546.root", //6
            
            
            
            
            
            
            // signal template, unbinned tree:
            "myTestSelector_signalTemplatesPlusUnbinned__1463496813.root", // 7
            // background templates, unbinned trees:
            //"myTestSelector_backgroundlTemplatesPlusUnbinned__1463495604.root", //8
            //"myTestSelector_backgroundlTemplatesPlusUnbinned__1464784114.root", // 8 muonPt > triggerTurnOn (40)
            "myTestSelector_backgroundlTemplatesPlusUnbinned__1468320274.root", // 8 muonPt > triggerTurnOn (40)
            // selected data, unbinned trees:
            //"myTestSelector_dataEvents_SingleElectron_plusUnbinned__1463502127.root", //9
            //"myTestSelector_dataEvents_SingleElectron_plusUnbinned__1468230878.root", // 9
            "myTestSelector_dataEvents_SingleElectron_plusUnbinned__1468316059.root", //9
            
            
            // multiple montecarlo (summed), unbinned tree with weight branch
            // DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v04.root
            // TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v03.root
            // WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v03.root
            // ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v03.root
            //"myTestSelector_mcEvents_DYJetsToLL+ZGTo2LNu+TTGJets+WGToLNuG+WJetsToLNu_plusUnbinned__1466678699.root", // 10
            "myTestSelector_mcEvents_DYJetsToLL+ZGTo2LG+TTGJets+WGToLNuG+WJetsToLNu_plusUnbinned__1468225385.root", // 10
            
            
            // only DY montecarlo, unbinned tree with weight branch
            "myTestSelector_mcEvents_DYJetsToLL_plusUnbinned__1466703587.root", //11
            
            // - background templates to scale the muon template for systematic uncertainty
            // - muon background template for monte carlo
            // - calculated on the multiple monte carlo stack from above
            // 
            // "myTestSelector_backgroundlTemplates_MC_PlusUnbinned__1468205891.root", // 12
            // 
            // weighted::
            //
            "myTestSelector_backgroundlTemplates_MC_stack_PlusUnbinned__1468495636.root", //12
            
            // background templates for each monte carlo sample
            "myTestSelector_backgroundlTemplates_MC_WJetsToLNu_PlusUnbinned__1468250515.root", //13 keine Trees
            "myTestSelector_backgroundlTemplates_MC_DYJetsToLL_PlusUnbinned__1468251383.root", //14 Trees
            "myTestSelector_backgroundlTemplates_MC_TTGJets_PlusUnbinned__1468254628.root", // 15   keine Trees
            "myTestSelector_backgroundlTemplates_MC_WGToLNuG_PlusUnbinned__1468255289.root", //16   keine Trees
            "myTestSelector_backgroundlTemplates_MC_ZGTo2LG_PlusUnbinned__1468255989.root", //17    keine Trees
            
            // closure plot file
            "myTestSelector_closure_montecarloStack__1468301810.root", //18
            
            // multiple montecarlos, unbinned trees with weight branches
            "myTestSelector_mcEvents_ZGTo2LG_plusUnbinned__1468403724.root", // 19      keine trees
            "myTestSelector_mcEvents_TTGJets_plusUnbinned__1468405068.root", // 20      ""
            "myTestSelector_mcEvents_WGToLNuG_plusUnbinned__1468405147.root", // 21     ""
            "myTestSelector_mcEvents_DYJetsToLL_plusUnbinned__1468403356.root", // 22   trees!
            "myTestSelector_mcEvents_WJetsToLNu_plusUnbinned__1468405457.root", // 23   keine trees
            "myTestSelector_mcEvents_TTJets_plusUnbinned__1468489085.root", // 24       trees!
            
            
            // monte carlo background samples in LO, ohne gewichte.
            "myTestSelector_backgroundlTemplates_MC_WJetsToLNu_PlusUnbinned__1469460716.root", // 25
            
            
            
            
        };
    
    
    
    dataset = dropbox + ss + filenames[2];
    
    cout << "dataset: " << dataset << endl;;
    //getchar();
    double start_time = time(NULL); // measure running time
    
    
    /* **********************************************
     * try some things
     * */
    // firstTry(dropbox+ss+filenames[2]);
    // secondTry(dropbox+ss+filenames[2]);
    // secondTry_eg_only(dropbox+ss+filenames[2]);
    // thirdTry(dropbox+ss+filenames[2]);
    
    // kernelEst(dropbox+ss+filenames[2]);
    
    // fourthTry(dataset);
    
    /* **********************************************
     * fit models from CMS TnP package
     * */
    // FitterEG(dropbox+ss+filenames[2]);
    // FitterEE(dropbox+ss+filenames[2]);
    
    // FitterEGdata(dropbox+ss+filenames[4]);
    // FitterEEdata(dropbox+ss+filenames[4]);
    
    // simplePlot(dropbox+ss+filenames[0], dropbox+ss+filenames[2]);
    
    // templateKernel(signal, bkg, data)
    // templateKernel(dropbox+ss+filenames[2],dropbox+ss+filenames[5],dropbox+ss+filenames[4]);
    
    // templateKernelTest(dropbox+ss+filenames[2],dropbox+ss+filenames[5],dropbox+ss+filenames[4]);
    
    // readValues();
    
    // templateKernelClass(dropbox+ss+filenames[2],dropbox+ss+filenames[5],dropbox+ss+filenames[4]);
    
    // templateKernelUnbinned(string signal, string background, string dataset)
    
    
    map<string, string> set;
    //
    set["templateSignal"]              = filenames[7];
    set["templateBackground"]          = filenames[8];
    set["templateBackgroundMC_stack"]  = filenames[12];
    set["eventsData"]                  = filenames[9];
    set["eventsMC_stack"]              = filenames[10];
    set["closureMC_stack"]             = filenames[18];
    // mc templates for background seperated
    set["templateBackgroundMC_WJetsToLNu"] = filenames[13];
    set["templateBackgroundMC_DYJetsToLL"] = filenames[14];
    set["templateBackgroundMC_TTGJets"]    = filenames[15];
    set["templateBackgroundMC_WGToLNuG"]   = filenames[16];
    set["templateBackgroundMC_ZGTo2LG"]    = filenames[17];
    set["templateBackgroundMC_TTJets"]     = "";               // << prozessiert
    // mc events seperated
    set["eventsMC_WJetsToLNu"] = filenames[23]; // WJetsToLNu  23
    set["eventsMC_DYJetsToLL"] = filenames[22]; // DYJetsToLL  22
    set["eventsMC_TTGJets"]    = filenames[20]; // TTGJets     20
    set["eventsMC_WGToLNuG"]   = filenames[21]; // WGToLNuG    21
    set["eventsMC_ZGTo2LG"]    = filenames[19]; // ZGTo2LG     19
    set["eventsMC_TTJets"]     = filenames[24]; // TTJets      24
    
    
    
    string mcBkgTemplates[] = {filenames[13], filenames[14], filenames[15], filenames[16], filenames[17]};
    
    
    workflowHandler(set);
    
    
                        //~ string signal, 
                        //~ string background, 
                        //~ string mcbackground, 
                        //~ string dataset, 
                        //~ string mcset,
                        //~ string *mcBkgTemplates,
                        //~ string closurefile)
    
    
    
    
    
    //templateKernelUnbinnedGlobal(dropbox+ss+filenames[7],dropbox+ss+filenames[8],dropbox+ss+filenames[9]);    
    //~ templateKernelUnbinned( dropbox+ss+filenames[7],
                            //~ dropbox+ss+filenames[8],
                            //~ dropbox+ss+filenames[9],
                            //~ dropbox+ss+filenames[11]
                            //~ ); 
                               
    //templateKernelUnbinned_PtNtrk(dropbox+ss+filenames[7],dropbox+ss+filenames[8],dropbox+ss+filenames[9]);
    
    
    //readFitResults(dropbox);
    
    //influenceBinningFit(dropbox);
    //influenceSignalBackground(dropbox);
    
    //cout << "...done" << endl;
    
    //test();
    
    double end_time = 1.*( time(NULL));
    cout << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
    
} // myTestSelector() */

# ifndef __CINT__
int main ( ) {
    rooFitter ( ) ;
    return 0 ;
}
# endif

