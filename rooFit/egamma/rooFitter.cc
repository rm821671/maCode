
#include "rooFitHeader.h"




void rooFitter(){
    
    //gROOT->Reset();
    //gSystem->Load("pluginTreeWriterTreeWriterAuto.so");
    
    string filepath, dropbox, ss, dataset;
    
    // filepath and dropbox path changes, depending on system
    SystemPath(filepath, dropbox);
    
    cout << "filepath: " << filepath << endl;
    cout << "dropbox: " << dropbox << endl;
    
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
            "myTestSelector_backgroundlTemplatesPlusUnbinned__1464784114.root", // 8 muonPt > triggerTurnOn (40)
            // selected data, unbinned trees:
            "myTestSelector_dataEvents_SingleElectron_plusUnbinned__1463502127.root", //9
            
            
            
            
        };
    
    
    dataset = dropbox + ss + filenames[2];
    
    cout << "dataset: " << dataset << endl;;
    //getchar();
    double start_time = time(NULL); // measure running time
    
    
    /* **********************************************
     * try some things
     * */
    //firstTry(dropbox+ss+filenames[2]);
    //secondTry(dropbox+ss+filenames[2]);
    //secondTry_eg_only(dropbox+ss+filenames[2]);
    //thirdTry(dropbox+ss+filenames[2]);
    
    //kernelEst(dropbox+ss+filenames[2]);
    
    //fourthTry(dataset);
    
    /* **********************************************
     * fit models from CMS TnP package
     * */
    //FitterEG(dropbox+ss+filenames[2]);
    //FitterEE(dropbox+ss+filenames[2]);
    
    //FitterEGdata(dropbox+ss+filenames[4]);
    //FitterEEdata(dropbox+ss+filenames[4]);
    
    //simplePlot(dropbox+ss+filenames[0], dropbox+ss+filenames[2]);
    
    // templateKernel(signal, bkg, data)
    //templateKernel(dropbox+ss+filenames[2],dropbox+ss+filenames[5],dropbox+ss+filenames[4]);
    
    //templateKernelTest(dropbox+ss+filenames[2],dropbox+ss+filenames[5],dropbox+ss+filenames[4]);
    
    //readValues();
    
    //templateKernelClass(dropbox+ss+filenames[2],dropbox+ss+filenames[5],dropbox+ss+filenames[4]);
    
    // templateKernelUnbinned(string signal, string background, string dataset)
    
    templateKernelUnbinned(dropbox+ss+filenames[7],dropbox+ss+filenames[8],dropbox+ss+filenames[9]);    
    //templateKernelUnbinned_PtNtrk(dropbox+ss+filenames[7],dropbox+ss+filenames[8],dropbox+ss+filenames[9]);
    
    
    //readFitResults(dropbox);
    
    //influenceBinningFit(dropbox);
    //influenceSignalBackground(dropbox);
    
    
    
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

