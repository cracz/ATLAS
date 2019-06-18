#include <iostream>
#include "plotUtils.h"

using namespace plotUtils;

int main(int argc, char* argv[])
{
  TString mode;   //sig (signal), bkg (background), over (overlay), or roc (ROC curve)
  TString fileName;

  if (argc != 3) { usage(); }
  else { mode = argv[1]; fileName = argv[2]; }

  if (!fileName.Contains(".root")) 
    { std::cout << "Second argument is not a root file!" << std::endl; exit(1); }

  /***Extract parameters from the file name***/
  Ssiz_t first = fileName.First('_'), last = fileName.Index("Pt",2,first,TString::kExact) - 1;
  TString jetTypeUntrim = fileName(first + 1, last - first);
  TString jetTypeTrim = (jetTypeUntrim.Copy()).ReplaceAll("_"," ");
  TString minPt = fileName(fileName.Index("Pt",2,last,TString::kExact) + 2, 4); if (minPt[0] == '0') { minPt = minPt.Remove(TString::kLeading, '0'); }
  TString maxPt = fileName(fileName.Index("Pt",2,last,TString::kExact) + 7, 4);
  TString topWin = fileName(fileName.Index("TW",2,last+12,TString::kExact) + 2, 2);
  TString wWin = fileName(fileName.Index("WW",2,last+12,TString::kExact) + 2, 2);
  TString runNum;
  std::cout << "Enter the run number: ";
  std::cin >> runNum;

  
  TTree *sigTree = 0;
  TTree *bkgTree = 0;
  TH1F *sigHisto = new TH1F("sigHisto", "Title", 200, -15, 15);
  TH1F *bkgHisto = new TH1F("bkgHisto", "Title2", 200, -15, 15);
  TCanvas *canvas = new TCanvas("canvas", "SD", 800, 600);
  TFile *file = TFile::Open(fileName);
  if (!file) { std::cout << "File was not opened!" << std::endl; exit(1); }

  if (mode.EqualTo("sig"))                  /***Make signal chi histogram***/
    {
      std::cout << "Making signal histogram..." << std::endl;
      
      Double_t dZero = 0, *totalEntries = &dZero;
      Int_t iZero = 0, *zeroEntries = &iZero;                               //Entries that are out of range

      buildSigHisto(file, sigTree, sigHisto, zeroEntries);
      sigHisto->Draw("HIST"); canvas->Update();                            //HIST turns off error bars
      

      //Stats box
      TPaveStats *ps = (TPaveStats*)sigHisto->FindObject("stats");
      ps->SetOptStat(1001111);                                            //Add integral to stat box

      //Text box
      TPaveText *pt = new TPaveText(.15,.65,.425,.9,"NDC");
      TString line1 = "Jet type: " + jetTypeTrim;
      //TString line2 = "Exclusive subjets: " + exclSubs;
      TString line3 = "Pt: " + minPt + " - " + maxPt;
      TString line4 = "W Mass Window: " + wWin;
      TString line5 = "t Mass Window: " + topWin;
      //TString line6 = "#chi = 0 Entries: "; line6 += *zeroEntries;
      pt->AddText(line1); //pt->AddText(line2); 
      pt->AddText(line3); pt->AddText(line4); pt->AddText(line5); //pt->AddText(line6);
      pt->SetTextSize(0.03);
      pt->Draw(); canvas->Update();

      if (minPt.Atoi() < 1000) minPt = '0' + minPt;

      TString outputName = "Signal"+runNum+"_"+jetTypeUntrim+"Pt"+minPt+"_"+maxPt+"_TW"+topWin+"_WW"+wWin+".png";
      std::cout << "Saving image..." << std::endl;
      canvas->Print(outputName, "png");
    }
  else if (mode.EqualTo("bkg"))             /***Make background histo***/
    {
      std::cout << "Making background histogram..." << std::endl;

      Double_t dZero = 0, *totalEntries = &dZero;
      Int_t iZero = 0, *zeroEntries = &iZero;                             //Entries that are out of range

      buildBkgHisto(file, bkgTree, bkgHisto, zeroEntries);
      bkgHisto->Draw("HIST"); canvas->Update();                           //HIST turns off error bars

      //Stats box
      TPaveStats *ps = (TPaveStats*)bkgHisto->FindObject("stats");
      ps->SetOptStat(1001111);                                            //Add integral to stat box

      //Text box
      TPaveText *pt = new TPaveText(.15,.65,.425,.9,"NDC");
      TString line1 = "Jet type: " + jetTypeTrim;
      //TString line2 = "Exclusive subjets: " + exclSubs;
      TString line3 = "Pt: " + minPt + " - " + maxPt;
      TString line4 = "W Mass Window: " + wWin;
      TString line5 = "t Mass Window: " + topWin;
      //TString line6 = "#chi = 0 Entries: "; line6 += *zeroEntries;
      pt->AddText(line1); //pt->AddText(line2); 
      pt->AddText(line3); pt->AddText(line4); pt->AddText(line5); //pt->AddText(line6);
      pt->SetTextSize(0.03);
      pt->Draw(); canvas->Update();
      
      if (minPt.Atoi() < 1000) minPt = '0' + minPt;

      TString outputName = "Background"+runNum+"_"+jetTypeUntrim+"Pt"+minPt+"_"+maxPt+"_TW"+topWin+"_WW"+wWin+".png";
      std::cout << "Saving image..." << std::endl;
      canvas->Print(outputName, "png");
    }
  else if (mode.EqualTo("over"))          /***Make overlay histogram***/
    {
      std::cout << "Making overlay histogram..." << std::endl;
      
      Int_t isZero = 0, *sigZeroEntries = &isZero;                       //Entries that are out of range
      Int_t ibZero = 0, *bkgZeroEntries = &ibZero;
      
      buildSigHisto(file, sigTree, sigHisto, sigZeroEntries);
      buildBkgHisto(file, bkgTree, bkgHisto, bkgZeroEntries);


      //The signal histo is used as the "base" for the overlay plot
      //                           main title        x axis                y axis
      sigHisto->SetTitle("Shower Decon. Histograms; log #chi; Fraction of Events (x#bf{10^{-2}})");
      sigHisto->SetName("Signal"); 
      sigHisto->SetLineColor(kBlue);
      sigHisto->GetYaxis()->SetTitleOffset(1.3);
      sigHisto->Draw("HIST"); canvas->Update();

      //Signal stats box
      TPaveStats *ps1 = (TPaveStats*)sigHisto->FindObject("stats");
      ps1->SetOptStat(1001111);
      ps1->SetX1NDC(0.7); ps1->SetX2NDC(0.9);
      ps1->SetY1NDC(0.5); ps1->SetY2NDC(0.75);
      ps1->SetTextColor(kBlue);
      canvas->Modified();

      //Background histogram
      bkgHisto->SetName("Background"); bkgHisto->SetTitle("");
      bkgHisto->SetLineColor(kRed);
      bkgHisto->Draw("][SAMESHIST"); canvas->Update();

      //Background stats box
      TPaveStats *ps2 = (TPaveStats*)bkgHisto->FindObject("stats");
      ps2->SetOptStat(1001111);
      ps2->SetX1NDC(0.7); ps2->SetX2NDC(0.9);
      ps2->SetY1NDC(0.25); ps2->SetY2NDC(0.5);
      ps2->SetTextColor(kRed);
      canvas->Modified();

      //Legend box
      TLegend *leg = new TLegend(.7,.75,.9,.9);
      leg->AddEntry(sigHisto, "Signal");
      leg->AddEntry(bkgHisto, "Background");
      leg->Draw(); canvas->Update();

      //Misc text box
      TPaveText *pt = new TPaveText(.15,.65,.425,.9,"NDC");
      TString line1 = "Jet type: " + jetTypeTrim;
      //TString line2 = "Exclusive subjets: " + exclSubs;
      TString line3 = "Pt: " + minPt + " - " + maxPt;
      TString line4 = "W Mass Window: " + wWin;
      TString line5 = "t Mass Window: " + topWin;
      //TString line6 = "Sig #chi = 0 Entries: "; line6 += *sigZeroEntries;
      //TString line7 = "Bkg #chi = 0 Entries: "; line7 += *bkgZeroEntries;
      pt->AddText(line1); //pt->AddText(line2); 
      pt->AddText(line3); pt->AddText(line4);
      pt->AddText(line5); //pt->AddText(line6); pt->AddText(line7);
      pt->SetTextSize(0.03);
      pt->Draw(); canvas->Update();

      if (minPt.Atoi() < 1000) minPt = '0' + minPt;

      TString outputName = "Overlay"+runNum+"_"+jetTypeUntrim+"Pt"+minPt+"_"+maxPt+"_TW"+topWin+"_WW"+wWin+".png";
      std::cout << "Saving image..." << std::endl;
      canvas->Print(outputName, ".png");
    }
  else if (mode.EqualTo("roc"))
    {
      std::cout << "Making ROC curve..." << std::endl;
      /*            
      getTree(file, sigTree, "Prime");
      getTree(file, bkgTree, "Dijet");
      */

      buildSigHisto(file, sigTree, sigHisto);                                         //Build histograms without finding the
      buildBkgHisto(file, bkgTree, bkgHisto);                                         //  the number of entries out of range

      Int_t maxBin = sigHisto->FindBin(15)+1;                            //Range is: -15 <= log(chi) <= 15
      Int_t minBin = sigHisto->FindBin(-15);
      Int_t totalBins = maxBin - minBin;
      
      Double_t xVals[totalBins];
      Double_t yVals[totalBins];

      fillAxes(sigHisto, bkgHisto, xVals, yVals);

      /*
      Int_t indexFlag = 0;
      Int_t trimmedSize = 0;
      
      //Find the point at which the signal eff. is too small to plot
      for (int i = 0; i < totalBins; ++i)
	{
	  if (xVals[i] < 1e-04) 
	    { 
	      indexFlag = i-1;
	      trimmedSize= i;
	      break;
	    }
	}
      
      Double_t xTrimmed[trimmedSize];
      Double_t yTrimmed[trimmedSize];
      
      //Filter out the points that have too small x values(and too high y values) to plot
      for (int i = 0; i < trimmedSize; ++i) { xTrimmed[i] = xVals[i]; yTrimmed[i] = yVals[i]; }
      */
      //Find important points
      Double_t zero1 = 0.0, zero2 = 0.0;
      Double_t maxSig = xVals[0], *bkg50Sig = &zero1, *bkg80Sig = &zero2;

      findAtt(xVals, yVals, totalBins, bkg50Sig, bkg80Sig);


      //Make ROC curve
      canvas->SetLogy();

      TGraph *roc = new TGraph(totalBins, xVals, yVals);
      roc->SetTitle("Run "+runNum+": "+jetTypeTrim+" Pt: "+minPt+" - "+maxPt);
      roc->GetXaxis()->SetTitle("Signal Efficiency"); roc->GetYaxis()->SetTitle("1 / (Background Efficiency)"); roc->GetYaxis()->SetTitleOffset(1.3);
      roc->SetLineWidth(2);
      roc->SetMinimum(1); roc->SetMaximum(10000);
      roc->Draw(); 
      roc->GetXaxis()->SetLimits(0,1);
      roc->Draw(); canvas->Update();

      TLine *line50 = new TLine(0, *bkg50Sig, 1, *bkg50Sig);
      line50->SetLineStyle(kDashed); line50->SetLineColor(kRed); line50->SetLineWidth(2);
      line50->Draw(); canvas->Update();

      TLine *line80 = new TLine(0, *bkg80Sig, 1, *bkg80Sig);
      line80->SetLineStyle(kDashed); line80->SetLineColor(kBlue); line80->SetLineWidth(2);
      line80->Draw(); canvas->Update();

      TLegend *leg = new TLegend(.65,.75,.9,.9);
      leg->AddEntry(line50, "50% Signal");
      leg->AddEntry(line80, "80% Signal");
      leg->SetTextSize(0.04);
      leg->Draw(); canvas->Update();

      /*
      TPaveText *pt = new TPaveText(.6, .65, .9, .9, "NDC");
      TString line1 = "Max sig efficiency: " + TString::Format("%0.2f", maxSig);
      TString line2 = "Background at 50% signal: ~" + TString::Format("%0.0f", *bkg50Sig);
      TString line3 = "Background at 80% signal: ~" + TString::Format("%0.0f", *bkg80Sig);
      pt->AddText(line1); pt->AddText(line2); pt->AddText(line3);
      //pt->SetTextSize(0.03);
      pt->Draw(); canvas->Update();
      */

      if (minPt.Atoi() < 1000) minPt = '0' + minPt;

      TString imageName = "ROC"+runNum+"_"+jetTypeUntrim+"Pt"+minPt+"_"+maxPt+"_TW"+topWin+"_WW"+wWin+".png";
      TString rocFileName = "ROC"+runNum+"_"+jetTypeUntrim+"Pt"+minPt+"_"+maxPt+"_TW"+topWin+"_WW"+wWin+".root";

      std::cout << "Saving root file..." << std::endl;
      TFile *f = TFile::Open(rocFileName,"RECREATE");
      f->cd();
      roc->Write("roc");
      f->ls();
      f->Close();

      std::cout << "Saving image..." << std::endl;      
      canvas->Print(imageName, "png");
    }
  else { std::cout << "Unrecognized Argument" << std::endl; usage(); }
  
  file->Close();

  std::cout << "Done" << std::endl;
  
  return 0;
}//End of main


//Old way of getting the file name

/*
  char *pathName = gSystem->ExpandPathName("$PLOTINPUT");                       //Get file and check validity
  if (gSystem->AccessPathName(pathName))
  { std::cout << "Specified file not found" << std::endl; exit(1); }
  
  TString pathNameStr = (TString)pathName;
  
  Ssiz_t lastSlash = pathNameStr.Last('/');
  TString fileName = pathNameStr.Remove(0, lastSlash + 1);
*/

//Old way of finding the jet type

/*
  switch (jetTypeNum.Atoi())
    {
    case 0: jetTypeString = "fastjet excl";
      break;
    case 1: jetTypeString = "CA R = 0.2";
      break;
    case 2: jetTypeString = "CA 3 or 0 subs";
      break;
    case 3: jetTypeString = "Akt R = 0.2";
      break;
    case 4: jetTypeString = "Kt R = 0.2";
      break;
    case 5: jetTypeString = "Topoclusters";
      break;
    }
*/
