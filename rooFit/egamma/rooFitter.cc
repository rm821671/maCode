
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
    
    string filenames[] = { "myTestSelector_NEW_tnp_forPlots_1460453023.root",  // 0
                                ///no b2b, higher resolution in Z mass spectrum
                            "myTestSelector_NEW_tnp_forPlots_1460459442.root",   // 1
                                /// back to back 0.8 pi to 1.2 pi
                            "myTestSelector_NEW_tnp_forPlots_1460464275.root", //2
                                /// back to back 0.9 pi to 1.1 pi
                            "myTestSelector_NEW_data_tnp_forPlots_1460633565.root", //3
                                /// complete data
                        
                            // "myTestSelector_NEW_tnp_forPlots_1460109853.root"
                        
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
    
    FitterEE(dropbox+ss+filenames[2]);
    
    //simplePlot(dropbox+ss+filenames[0], dropbox+ss+filenames[2]);
    
    double end_time = 1.*( time(NULL));
    cout << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
    
} // myTestSelector() */


