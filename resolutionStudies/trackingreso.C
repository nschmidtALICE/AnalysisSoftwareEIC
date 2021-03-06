#include "plottingheader.h"
#define NELEMS(arr) (sizeof(arr)/sizeof(arr[0]))
void split_canvas(TCanvas* &cPNG, TString canvName, Int_t numInputs);

void trackingreso(
    TString suffix            = "pdf"
){

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  StyleSettingsThesis();
  TString dateForOutput                       = ReturnDateStringForOutput();

  TString outputDir 						                  = Form("plots/%s",dateForOutput.Data());
  gSystem->Exec("mkdir -p "+outputDir);

  Double_t textSizeSinglePad                    = 0.05;
  Double_t textSizeLabelsPixel                    = 35;
  Double_t textSizeLabelsRel                    = 58./1300;


  //************************** Read data **************************************************
  const Int_t nEne = 23;
  const static Double_t partE[]   = {3.0, 4.0, 4.5, 5.0, 5.5, 6.0, 7.0, 7.5, 8.0, 9.0,  10., 11., 13., 15., 20., 25., 30., 40., 50., 60., 70., 80., 150.};
  Int_t useE[nEne]                = { 0};
  Int_t activeE = {0};

  TH1F* h_tracks_mean_eta_reso[nEne] 	    = {NULL};
  TH1F* h_tracks_sigma_eta_reso[nEne] 	    = {NULL};
  TH2F* h_tracks_reso[nEne] 	    = {NULL};
  TH2F* h_tracks_reso_Eta[nEne] 	    = {NULL};
  TH2F* h_tracks_nhits_Eta[nEne] 	    = {NULL};
  TH2F* h_tracks_nhits_pT[nEne] 	    = {NULL};
  Double_t etalowdef = 3.0;
  Double_t etahighdef = 4.0;

    for(Int_t epart=0; epart<NELEMS(partE); epart++){
      cout << partE[epart] << endl;

      TChain* t_tracks = new TChain("tracks", "tracks");
      t_tracks->Add(Form("/media/nschmidt/local/EIC_running/cluster_output/%s/800_%1.1f/G4EICDetector_g4tracking_eval.root","Fun4All_G4_FullDetector_Pythia_beampipe",partE[epart]));
      for(Int_t ii=1; ii<4;ii++){
          t_tracks->Add(Form("/media/nschmidt/external2/EICsimulationOutputs/%s/800_%d_%1.1f/G4EICDetector_g4tracking_eval.root","Fun4All_G4_FullDetector_Pythia_beampipe",ii,partE[epart]));
      }

      if(t_tracks){
        useE[epart] = 1;
        activeE++;
        h_tracks_reso[epart] 	= new TH2F(Form("h_tracks_reso_%d",epart), "", 200, 0, 40, 200, -1, 1);
        h_tracks_reso_Eta[epart] 	= new TH2F(Form("h_tracks_reso_%d",epart), "", 20, 0, 4, 200, -1, 1);
        h_tracks_nhits_Eta[epart] 	= new TH2F(Form("h_tracks_reso_%d",epart), "", 20, 0, 4, 10, -0.5, 9.5);
        h_tracks_nhits_pT[epart] 	= new TH2F(Form("h_tracks_reso_%d",epart), "", 200, 0, 40, 10, -0.5, 9.5);

        Float_t trk_px,trk_py,trk_pz,trk_px_true,trk_py_true,trk_pz_true;
        Int_t trk_hits;
        t_tracks->SetBranchAddress("px", &trk_px);
        t_tracks->SetBranchAddress("py", &trk_py);
        t_tracks->SetBranchAddress("pz", &trk_pz);
        t_tracks->SetBranchAddress("gpx", &trk_px_true);
        t_tracks->SetBranchAddress("gpy", &trk_py_true);
        t_tracks->SetBranchAddress("gpz", &trk_pz_true);
        t_tracks->SetBranchAddress("nhits", &trk_hits);

        Int_t nentries = Int_t(t_tracks->GetEntries());
        Float_t lastEvt = 0;
        for (Int_t i = 0; i < nentries; ++i) {
          if (t_tracks->LoadTree(i) < 0)
            break;

          t_tracks->GetEntry(i);
          TVector3 vec_mom_true(trk_px, trk_py, trk_pz);
          TVector3 vec_mom(trk_px_true, trk_py_true, trk_pz_true);

          // if (((true_eta1 > 1.7 && true_eta1 < 3.0)
          //   || (true_eta2 > 1.7 && true_eta2 < 3.0))  //1.7 < 3
          // && (measured_energy1 > 0.1) // above MIP
          // && (measured_energy1 > 0.1) // above MIP
          // && nTowers1>=2 && nTowers2>=2 // actual cluster with >1 tower
          // ){
            h_tracks_reso[epart]->Fill(TMath::Sqrt(TMath::Power(trk_px_true,2)+TMath::Power(trk_py_true,2)),(TMath::Sqrt(TMath::Power(trk_px,2)+TMath::Power(trk_py,2))-TMath::Sqrt(TMath::Power(trk_px_true,2)+TMath::Power(trk_py_true,2)))/TMath::Sqrt(TMath::Power(trk_px_true,2)+TMath::Power(trk_py_true,2)));
            h_tracks_reso_Eta[epart]->Fill(vec_mom_true.Eta(),(TMath::Sqrt(TMath::Power(trk_px,2)+TMath::Power(trk_py,2))-TMath::Sqrt(TMath::Power(trk_px_true,2)+TMath::Power(trk_py_true,2)))/TMath::Sqrt(TMath::Power(trk_px_true,2)+TMath::Power(trk_py_true,2)));

            h_tracks_nhits_Eta[epart]->Fill(TMath::Sqrt(TMath::Power(trk_px_true,2)+TMath::Power(trk_py_true,2)),trk_hits);
            h_tracks_nhits_pT[epart]->Fill(vec_mom_true.Eta(),trk_hits);
          // }
        }
        h_tracks_reso[epart]->Scale(1 / h_tracks_reso[epart]->GetEntries());
        h_tracks_reso_Eta[epart]->Scale(1 / h_tracks_reso_Eta[epart]->GetEntries());
        h_tracks_nhits_Eta[epart]->Scale(1 / h_tracks_nhits_Eta[epart]->GetEntries());
        h_tracks_nhits_pT[epart]->Scale(1 / h_tracks_nhits_pT[epart]->GetEntries());

        h_tracks_mean_eta_reso[epart] = new TH1F(Form("h_tracks_mean_eta_reso%d",epart),"",20, 0, 4);
        h_tracks_sigma_eta_reso[epart] = new TH1F(Form("h_tracks_sigma_eta_reso%d",epart),"",20, 0, 4);
        for (Int_t i=1; i < h_tracks_reso_Eta[epart]->GetNbinsX(); i++){
          TH1D* projectionYdummy = (TH1D*)h_tracks_reso_Eta[epart]->ProjectionY(Form("projectionYdummy%d%d",i,epart), i,i+1,"e");
          h_tracks_mean_eta_reso[epart]->SetBinContent(i,projectionYdummy->GetMean());
          h_tracks_mean_eta_reso[epart]->SetBinError(i,projectionYdummy->GetMeanError());
          h_tracks_sigma_eta_reso[epart]->SetBinContent(i,projectionYdummy->GetStdDev()); //GetMeanError()
          h_tracks_sigma_eta_reso[epart]->SetBinError(i,projectionYdummy->GetRMSError()); //GetMeanError()
        }
      }
    }



  TCanvas* cPNG; // = new TCanvas("cPNG", "", gStyle->GetCanvasDefW() * 3,
	split_canvas(cPNG, "cPNG1", activeE);
  Int_t padnum =0;
  for(Int_t epart=0; epart<NELEMS(partE); epart++){
    if(!useE[epart]) continue;
    cPNG->cd(padnum+1);

    SetStyleHistoTH2ForGraphs(h_tracks_reso[epart], "#it{p}_{T}^{true}","(#it{p}_{T}^{rec}-#it{p}_{T}^{true})/#it{p}_{T}^{true}", 0.85*textSizeSinglePad,textSizeSinglePad, 0.85*textSizeSinglePad,textSizeSinglePad, 0.9,0.81);

    h_tracks_reso[epart]->Draw("colz");
    drawLatexAdd(Form("%1.1f GeV #pi^{-} in FST",partE[epart]),0.90,0.85,1.3*textSizeLabelsRel,kFALSE,kFALSE,kTRUE);
    DrawGammaLines(0, 40, 0., 0., 1, kGray+2, 2);

    padnum++;
  }
  cPNG->Print(Form("%s/Tracking_Resolution.%s", outputDir.Data(), suffix.Data()));

	split_canvas(cPNG, "cPNG1", activeE);
  padnum =0;
  for(Int_t epart=0; epart<NELEMS(partE); epart++){
    if(!useE[epart]) continue;
    cPNG->cd(padnum+1);
    SetStyleHistoTH2ForGraphs(h_tracks_reso_Eta[epart], "#eta^{true}","(#it{p}_{T}^{rec}-#it{p}_{T}^{true})/#it{p}_{T}^{true}", 0.85*textSizeSinglePad,textSizeSinglePad, 0.85*textSizeSinglePad,textSizeSinglePad, 0.9,0.81);

    h_tracks_reso_Eta[epart]->Draw("colz,same");
    drawLatexAdd(Form("%1.1f GeV #pi^{-} in FST",partE[epart]),0.90,0.85,1.3*textSizeLabelsRel,kFALSE,kFALSE,kTRUE);
    DrawGammaLines(0, 40, 0., 0., 1, kGray+2, 2);

    padnum++;
  }
  cPNG->Print(Form("%s/Tracking_Resolution_Eta.%s", outputDir.Data(), suffix.Data()));

	split_canvas(cPNG, "cPNG1", activeE);
  padnum =0;
  for(Int_t epart=0; epart<NELEMS(partE); epart++){
    if(!useE[epart]) continue;
    cPNG->cd(padnum+1);
    SetStyleHistoTH2ForGraphs(h_tracks_nhits_Eta[epart], "#eta^{true}","num. hits", 0.85*textSizeSinglePad,textSizeSinglePad, 0.85*textSizeSinglePad,textSizeSinglePad, 0.9,0.81);

    h_tracks_nhits_Eta[epart]->Draw("colz,same");
    drawLatexAdd(Form("%1.1f GeV #pi^{-} in FST",partE[epart]),0.90,0.85,1.3*textSizeLabelsRel,kFALSE,kFALSE,kTRUE);
    DrawGammaLines(0, 40, 0., 0., 1, kGray+2, 2);

    padnum++;
  }
  cPNG->Print(Form("%s/Tracking_NHits_Eta.%s", outputDir.Data(), suffix.Data()));

  TH2F * histTrackResoMeanSigDummy                           = new TH2F("histTrackResoMeanSigDummy","histTrackResoMeanSigDummy",200, 0, 4,1000,-0.1, 0.4);
  SetStyleHistoTH2ForGraphs(histTrackResoMeanSigDummy, "#eta_{rec}","(#sigma)(#it{p}_{T}^{rec}-#it{p}_{T}^{true})/#it{p}_{T}^{true}", 0.85*textSizeSinglePad,textSizeSinglePad, 0.85*textSizeSinglePad,textSizeSinglePad, 0.9,0.81);
  histTrackResoMeanSigDummy->GetXaxis()->SetNoExponent();
  histTrackResoMeanSigDummy->GetYaxis()->SetNdivisions(505,kTRUE);
  histTrackResoMeanSigDummy->GetXaxis()->SetMoreLogLabels(kTRUE);
	split_canvas(cPNG, "cPNG1", activeE);
  padnum =0;
  for(Int_t epart=0; epart<NELEMS(partE); epart++){
    if(!useE[epart]) continue;
    cPNG->cd(padnum+1);
    histTrackResoMeanSigDummy->DrawCopy();
    TLegend* legendMeanSig  = GetAndSetLegend2(0.17, 0.83-(3*textSizeLabelsRel), 0.6, 0.83,1.0*textSizeLabelsPixel, 1, "", 43, 0.15);

    DrawGammaSetMarker(h_tracks_mean_eta_reso[epart], 47, 2, kBlue+2, kBlue+2);
    DrawGammaSetMarker(h_tracks_sigma_eta_reso[epart], 20, 2, kOrange+2, kOrange+2);

    h_tracks_mean_eta_reso[epart]->Draw("colz,same");
    h_tracks_sigma_eta_reso[epart]->Draw("colz,same");
    drawLatexAdd(Form("%1.1f GeV #pi^{-} in FST",partE[epart]),0.90,0.85,1.3*textSizeLabelsRel,kFALSE,kFALSE,kTRUE);
    DrawGammaLines(0, 40, 0., 0., 1, kGray+2, 2);

    legendMeanSig->AddEntry(h_tracks_mean_eta_reso[epart],Form("mean"),"p");
    legendMeanSig->AddEntry(h_tracks_sigma_eta_reso[epart],Form("sigma"),"p");
    legendMeanSig->Draw();

    padnum++;
  }
  cPNG->Print(Form("%s/Tracking_Resolution_Eta_MeanSig.%s", outputDir.Data(), suffix.Data()));


}


void split_canvas(TCanvas* &cPNG, TString canvName, Int_t numInputs)
{
  if(numInputs<5){
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 2, gStyle->GetCanvasDefH()*2);
	  cPNG->Divide(2, 2);
  }else if(numInputs<7){
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 3, gStyle->GetCanvasDefH()*2);
	  cPNG->Divide(3, 2);
  }else if(numInputs<10){
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 3, gStyle->GetCanvasDefH()*3);
	  cPNG->Divide(3, 3);
  } else if(numInputs<13){
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 4, gStyle->GetCanvasDefH()*3);
	  cPNG->Divide(4, 3);
  } else if(numInputs<17){
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 4, gStyle->GetCanvasDefH()*4);
	  cPNG->Divide(4, 4);
  } else if(numInputs<21){
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 5, gStyle->GetCanvasDefH()*4);
	  cPNG->Divide(5, 4);
  } else {
    cPNG = new TCanvas(canvName.Data(), "", gStyle->GetCanvasDefW() * 5, gStyle->GetCanvasDefH()*5);
	  cPNG->Divide(5, 5);
  }

}
