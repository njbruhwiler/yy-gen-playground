// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/ChargedFinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/DressedLeptons.hh"
#include "Rivet/Projections/MissingMomentum.hh"
#include "Rivet/Projections/DirectFinalState.hh"
#include "Rivet/Projections/VetoedFinalState.hh"

namespace Rivet {


  /// @brief Add a short analysis description here
  class RivetAnalysis_yy : public Analysis {
  public:

    /// Constructor
    RIVET_DEFAULT_ANALYSIS_CTOR(RivetAnalysis_yy);


    /// @name Analysis methods
    /// @{

    /// Book histograms and initialise projections before the run
    void init() {

      // Initialise and register projections

      // The basic final-state projection:
      // all final-state particles within
      // the given eta acceptance
      const FinalState fs(Cuts::abseta < 5.0 && Cuts::pT > 500*MeV);
      declare(fs, "FS");

      // FinalState of direct photons and bare muons and electrons in the event
      DirectFinalState photons(Cuts::abspid == PID::PHOTON);
      DirectFinalState bare_leps(Cuts::abspid == PID::MUON || Cuts::abspid == PID::ELECTRON);

      // Dress the bare direct leptons with direct photons within dR < 0.1,
      // and apply some fiducial cuts on the dressed leptons
      Cut lepton_cuts = Cuts::abseta < 2.5 && Cuts::pT > 10*GeV;
      DressedLeptons dressed_leps(photons, bare_leps, 0.1, lepton_cuts);
      declare(dressed_leps, "leptons");

      // Remove taus
      VetoedFinalState bare_leps_no_tau(bare_leps);
      bare_leps_no_tau.addDecayProductsVeto(PID::TAU);
      bare_leps_no_tau.addDecayProductsVeto(-PID::TAU);
      declare(bare_leps_no_tau, "leps_no_tau");

      // DressedLeptons dressed_leps(photons, bare_leps, 0.1, lepton_cuts);
      // declare(dressed_leps, "leptons");
      DressedLeptons dressed_leps_no_tau(photons, bare_leps_no_tau, 0.1, lepton_cuts);
      declare(dressed_leps_no_tau, "dressed_leptons_no_tau");

      //Charged particles
      const ChargedFinalState cfs(fs);
      declare(cfs, "CFS");

      //Veto charged particle tracks which belong to the leptons
      VetoedFinalState cfs_nolep(cfs); 
      cfs_nolep.addVetoPair(PID::ELECTRON, lepton_cuts); 
      cfs_nolep.addVetoPair(PID::MUON, lepton_cuts);
      declare(cfs_nolep, "CFS_NL");

      // The final-state particles declared above are clustered using FastJet with
      // the anti-kT algorithm and a jet-radius parameter 0.4
      // muons and neutrinos are excluded from the clustering
      FastJets jetfs(fs, FastJets::ANTIKT, 0.4, JetAlg::Muons::NONE, JetAlg::Invisibles::NONE);
      declare(jetfs, "jets");

      // Missing momentum
      declare(MissingMomentum(fs), "MET");

      // Book histograms
      // specify custom binning
      book(_histMult   , "Mult", 100, -0.5, 99.5);
      book(_histMultCh , "MultCh", 100, -0.5, 99.5);

      book(_histPt   , "Pt", 300, 0, 300);
      book(_histPtCh , "PtCh", 300, 0, 300);

      book(_histE   , "E", 100, 0, 200);
      book(_histECh , "ECh", 100, 0, 200);

      book(_histEtaSumEt , "EtaSumEt", 25, 0, 5);

      book(_histEta    , "Eta", 50, -5, 5);
      book(_histEtaCh  , "EtaCh", 50, -5, 5);
      book(_tmphistEtaPlus, "TMP/EtaPlus", 25, 0, 5);
      book(_tmphistEtaMinus, "TMP/EtaMinus", 25, 0, 5);
      book(_tmphistEtaChPlus, "TMP/EtaChPlus", 25, 0, 5);
      book(_tmphistEtaChMinus, "TMP/EtaChMinus", 25, 0, 5);

      book(_histRapidity    , "Rapidity", 50, -5, 5);
      book(_histRapidityCh  , "RapidityCh", 50, -5, 5);
      book(_tmphistRapPlus, "TMP/RapPlus", 25, 0, 5);
      book(_tmphistRapMinus, "TMP/RapMinus", 25, 0, 5);
      book(_tmphistRapChPlus, "TMP/RapChPlus", 25, 0, 5);
      book(_tmphistRapChMinus, "TMP/RapChMinus", 25, 0, 5);

      book(_histPhi    , "Phi", 50, 0, TWOPI);
      book(_histPhiCh  , "PhiCh", 50, 0, TWOPI);

      book(_histEtaPMRatio , "EtaPMRatio");
      book(_histEtaChPMRatio , "EtaChPMRatio");
      book(_histRapidityPMRatio , "RapidityPMRatio");
      book(_histRapidityChPMRatio , "RapidityChPMRatio");

      // Histograms Leptons
      book(_histLeptonPt1, "ptlepton1", 100, 0, 100);
      book(_histLeptonPt2, "ptlepton2", 100, 0, 100);
      book(_histLeptonEta1, "etalepton1", 50, -2.5, 2.5);
      book(_histLeptonEta2, "etalepton2", 50, -2.5, 2.5);

      // Histograms di-lepton system
      book(_histMll, "mll", 500, 0, 500);
      book(_histPtll, "ptll", 500, 0, 500);
      book(_histDphill, "Dphill", 64, -3.2, 3.2);
      book(_histAcoll, "Acoll", 100, 0, 1.0);
      // book(_histLogAcoll, "log(Aco) ll", 100, -10, 0);

      // Histograms charged particles w/o leptons
      book(_histMultChNL, "MultCh ", 50, -0.5, 49.5);
      book(_histPtChNL, "PtCh ", 500, 0, 500); 
      book(_histEtaChNL, "EtaCh ", 100, -5.0, 5.0);

/*    //examples
      book(_h["XXXX"], "myh1", 20, 0.0, 100.0);
      book(_h["YYYY"], "myh2", logspace(20, 1e-2, 1e3));
      book(_h["ZZZZ"], "myh3", {0.0, 1.0, 2.0, 4.0, 8.0, 16.0});
      // take binning from reference data using HEPData ID (digits in "d01-x01-y01" etc.)
      book(_h["AAAA"], 1, 1, 1);
      book(_p["BBBB"], 2, 1, 1);
      book(_c["CCCC"], 3, 1, 1);
*/
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {

      // Retrieve dressed leptons, sorted by pT
      vector<DressedLepton> leptons = apply<DressedLeptons>(event, "leptons").dressedLeptons();

      // Retrieve clustered jets, sorted by pT, with a minimum pT cut
      //Jets jets = apply<FastJets>(event, "jets").jetsByPt(Cuts::pT > 30*GeV);

      // Remove all jets within dR < 0.2 of a dressed lepton
      //idiscardIfAnyDeltaRLess(jets, leptons, 0.2);

      // Select jets ghost-associated to B-hadrons with a certain fiducial selection
      //Jets bjets = filter_select(jets, [](const Jet& jet) {
      //  return  jet.bTagged(Cuts::pT > 5*GeV && Cuts::abseta < 2.5);
      //});

      // Veto event if there are no b-jets
      //if (bjets.empty())  vetoEvent;

      // Apply a missing-momentum cut
      //if (apply<MissingMomentum>(event, "MET").missingPt() < 30*GeV)  vetoEvent;

      // Fill histogram with leading b-jet pT
      //_h["XXXX"]->fill(bjets[0].pT()/GeV);

      // Charged + neutral final state
      const FinalState& fs = apply<FinalState>(event, "FS");

      // Charged FS particles only
      const FinalState& cfs_withlep = apply<FinalState>(event, "CFS");
      const FinalState& cfs_nolep = apply<FinalState>(event, "CFS_NL");

      Particles cfs_nolep_bypt = cfs_nolep.particles(cmpMomByPt); 

      const FourMomentum dilepton = leptons.size()>1 ? leptons[0].momentum() + leptons[1].momentum() : FourMomentum(0,0,0,0);
      
      const FourMomentum l1 = leptons.size()>0 ? leptons[0].momentum() : FourMomentum(0,0,0,0);
      const FourMomentum l2 = leptons.size()>1 ? leptons[1].momentum() : FourMomentum(0,0,0,0);

      const double Etal1 = leptons.size()>0 ? l1.eta() : -15;
      const double Etal2 = leptons.size()>1 ? l2.eta() : -15;

      const double mll = leptons.size()>1 ? dilepton.mass()/GeV : -1;
      const double pTll = leptons.size()>1 ? dilepton.pT()/GeV : -1;
      const double dphill = leptons.size()>1 ? fabs(deltaPhi(leptons[0], leptons[1])) : -1;
      const double aco = leptons.size()>1 ? fabs(1 - ((leptons[0].phi() - leptons[1].phi())/PI)): -1;

      const double Ptl1 = leptons.size()>0 ? l1.pT()/GeV : -1;
      const double Ptl2 = leptons.size()>1 ? l2.pT()/GeV : -1;

      // All FS particles
      _histMult->fill(fs.size());

      for (const Particle& p : fs.particles()) {
        _histEta->fill(p.eta());
        _histEtaSumEt->fill(p.abseta(), p.Et());
        (p.eta() > 0 ? _tmphistEtaPlus : _tmphistEtaMinus)->fill(p.abseta());
        //
        _histRapidity->fill(p.rap());
        (p.rap() > 0 ? _tmphistRapPlus : _tmphistRapMinus)->fill(p.absrap());
        //
        _histPt->fill(p.pT()/GeV);
        _histE->fill(p.E()/GeV);
        _histPhi->fill(p.phi());
      }

      // Same for the charged FS particles only
      const FinalState& cfs = apply<FinalState>(event, "CFS");
      MSG_DEBUG("Total charged multiplicity = " << cfs.size());
      _histMultCh->fill(cfs.size());
      for (const Particle& p : cfs.particles()) {
        _histEtaCh->fill(p.eta());
        (p.eta() > 0 ? _tmphistEtaChPlus : _tmphistEtaChMinus)->fill(p.abseta());
        //
        _histRapidityCh->fill(p.rap());
        (p.rap() > 0 ? _tmphistRapChPlus : _tmphistRapChMinus)->fill(p.absrap());
        //
        _histPtCh->fill(p.pT()/GeV);
        _histECh->fill(p.E()/GeV);
        _histPhiCh->fill(p.phi());
      }

      // Selection 
      // veto events if no 2 leptons
      if (leptons.size()!=2) vetoEvent;

      // same or opposite flavor leptons
      //if (leptons[0].abspid() != leptons[1].abspid()) vetoEvent;
      //if (leptons[0].abspid() = leptons[1].abspid()) vetoEvent;

      // opposite charged leptons
      if (leptons[0].charge() == leptons[1].charge()) vetoEvent;

      // di-lepton invariant mass selection
      //if (mll <= 110) vetoEvent;


      // Fill histograms
      _histMll ->fill(mll);
      _histPtll -> fill(pTll);
      _histDphill -> fill(dphill);
      _histAcoll -> fill(aco);

      _histLeptonPt1 -> fill(Ptl1);
      _histLeptonPt2 -> fill(Ptl2);
      _histLeptonEta1 -> fill(Etal1);
      _histLeptonEta2 -> fill(Etal2);

      _histMultChNL->fill(cfs_nolep.size());
      for (const Particle& p : cfs_nolep.particles()) {
        _histEtaChNL->fill(p.eta());
        _histPtChNL->fill(p.pT()/GeV);
      }

    }


    /// Normalise histograms etc., after the run
    void finalize() {

      double normfac = crossSection()/sumOfWeights();

      scale(_histMll, normfac);
      scale(_histPtll, normfac);
      scale(_histDphill, normfac);
      scale(_histAcoll, normfac);

      scale(_histLeptonPt1, normfac);
      scale(_histLeptonPt2, normfac);
      scale(_histLeptonEta1, normfac);
      scale(_histLeptonEta2, normfac);

      scale(_histMultChNL, normfac);
      scale(_histPtChNL, normfac);
      scale(_histEtaChNL, normfac);

      normalize(_histMult); normalize(_histEta); normalize(_histRapidity); 
      normalize(_histPt); normalize(_histE); normalize(_histPhi);
      normalize(_histMultCh); normalize(_histEtaCh); normalize(_histRapidityCh); 
      normalize(_histPtCh); normalize(_histECh); normalize(_histPhiCh);
      divide(_tmphistEtaPlus, _tmphistEtaMinus, _histEtaPMRatio);
      divide(_tmphistEtaChPlus, _tmphistEtaChMinus, _histEtaChPMRatio);
      divide(_tmphistRapPlus, _tmphistRapMinus, _histRapidityPMRatio);
      divide(_tmphistRapChPlus, _tmphistRapChMinus, _histRapidityChPMRatio);

/*    //example
      normalize(_h["XXXX"]); // normalize to unity
      normalize(_h["YYYY"], crossSection()/picobarn); // normalize to generated cross-section in pb (no cuts)
      scale(_h["ZZZZ"], crossSection()/picobarn/sumW()); // norm to generated cross-section in pb (after cuts)
*/

    }

    /// @}


    /// @name Histograms
    /// @{

    Histo1DPtr _histMll, _histPtll, _histDphill, _histAcoll;
    Histo1DPtr _histLeptonPt1, _histLeptonPt2, _histLeptonEta1, _histLeptonEta2;
    Histo1DPtr _histMultChNL, _histPtChNL, _histEtaChNL;

    Histo1DPtr _histMult, _histEta, _histRapidity, _histPt, _histE, _histPhi;
    Histo1DPtr _histMultCh,  _histEtaCh, _histRapidityCh, _histPtCh, _histECh, _histPhiCh;
    Profile1DPtr _histEtaSumEt;
    Scatter2DPtr _histEtaPMRatio, _histEtaChPMRatio, _histRapidityPMRatio, _histRapidityChPMRatio;

    Histo1DPtr _tmphistEtaPlus, _tmphistEtaMinus, _tmphistEtaChPlus, _tmphistEtaChMinus;
    Histo1DPtr _tmphistRapPlus, _tmphistRapMinus, _tmphistRapChPlus, _tmphistRapChMinus;

    map<string, Histo1DPtr> _h;
    map<string, Profile1DPtr> _p;
    map<string, CounterPtr> _c;

    /// @}


  };


  RIVET_DECLARE_PLUGIN(RivetAnalysis_yy);

}

