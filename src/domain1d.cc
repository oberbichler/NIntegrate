#include "domain1d.h"

namespace domain1d {

const IntegrationPoints normPoints(const int &degree) {
    switch (degree) {
        case 1:
            return {
                IntegrationPoint( 0.00000000000000000, 2.00000000000000000)
            };
        case 2:
            return {
                IntegrationPoint(-0.57735026918962570, 1.00000000000000000),
                IntegrationPoint( 0.57735026918962570, 1.00000000000000000)
            };
        case 3:
            return {
                IntegrationPoint(-0.77459666924148340, 0.55555555555555570),
                IntegrationPoint( 0.00000000000000000, 0.88888888888888880),
                IntegrationPoint( 0.77459666924148340, 0.55555555555555570)
            };
        case 4:
            return {
                IntegrationPoint(-0.86113631159405260, 0.34785484513745370),
                IntegrationPoint(-0.33998104358485626, 0.65214515486254620),
                IntegrationPoint( 0.33998104358485626, 0.65214515486254620),
                IntegrationPoint( 0.86113631159405260, 0.34785484513745370)
            };
        case 5:
            return {
                IntegrationPoint(-0.90617984593866400, 0.23692688505618942),
                IntegrationPoint(-0.53846931010568310, 0.47862867049936620),
                IntegrationPoint( 0.00000000000000000, 0.56888888888888900),
                IntegrationPoint( 0.53846931010568310, 0.47862867049936620),
                IntegrationPoint( 0.90617984593866400, 0.23692688505618942)
            };
        case 6:
            return {
                IntegrationPoint(-0.93246951420315200, 0.17132449237916975),
                IntegrationPoint(-0.66120938646626450, 0.36076157304813894),
                IntegrationPoint(-0.23861918608319693, 0.46791393457269137),
                IntegrationPoint( 0.23861918608319693, 0.46791393457269137),
                IntegrationPoint( 0.66120938646626450, 0.36076157304813894),
                IntegrationPoint( 0.93246951420315200, 0.17132449237916975)
            };
        case 7:
            return {
                IntegrationPoint(-0.94910791234275850, 0.12948496616887065),
                IntegrationPoint(-0.74153118559939450, 0.27970539148927660),
                IntegrationPoint(-0.40584515137739720, 0.38183005050511830),
                IntegrationPoint( 0.00000000000000000, 0.41795918367346896),
                IntegrationPoint( 0.40584515137739720, 0.38183005050511830),
                IntegrationPoint( 0.74153118559939450, 0.27970539148927660),
                IntegrationPoint( 0.94910791234275850, 0.12948496616887065)
            };
        case 8:
            return {
                IntegrationPoint(-0.96028985649753620, 0.10122853629037669),
                IntegrationPoint(-0.79666647741362670, 0.22238103445337434),
                IntegrationPoint(-0.52553240991632900, 0.31370664587788705),
                IntegrationPoint(-0.18343464249564978, 0.36268378337836177),
                IntegrationPoint( 0.18343464249564978, 0.36268378337836177),
                IntegrationPoint( 0.52553240991632900, 0.31370664587788705),
                IntegrationPoint( 0.79666647741362670, 0.22238103445337434),
                IntegrationPoint( 0.96028985649753620, 0.10122853629037669)
            };
        case 9:
            return {
                IntegrationPoint(-0.96816023950762610, 0.08127438836157472),
                IntegrationPoint(-0.83603110732663580, 0.18064816069485712),
                IntegrationPoint(-0.61337143270059040, 0.26061069640293566),
                IntegrationPoint(-0.32425342340380890, 0.31234707704000280),
                IntegrationPoint( 0.00000000000000000, 0.33023935500125967),
                IntegrationPoint( 0.32425342340380890, 0.31234707704000280),
                IntegrationPoint( 0.61337143270059040, 0.26061069640293566),
                IntegrationPoint( 0.83603110732663580, 0.18064816069485712),
                IntegrationPoint( 0.96816023950762610, 0.08127438836157472)
            };
        case 10:
            return {
                IntegrationPoint(-0.97390652851717170, 0.06667134430868807),
                IntegrationPoint(-0.86506336668898450, 0.14945134915058036),
                IntegrationPoint(-0.67940956829902440, 0.21908636251598200),
                IntegrationPoint(-0.43339539412924720, 0.26926671930999650),
                IntegrationPoint(-0.14887433898163122, 0.29552422471475300),
                IntegrationPoint( 0.14887433898163122, 0.29552422471475300),
                IntegrationPoint( 0.43339539412924720, 0.26926671930999650),
                IntegrationPoint( 0.67940956829902440, 0.21908636251598200),
                IntegrationPoint( 0.86506336668898450, 0.14945134915058036),
                IntegrationPoint( 0.97390652851717170, 0.06667134430868807)
            };
        case 11:
            return {
                IntegrationPoint(-0.97822865814605700, 0.055668567116173164),
                IntegrationPoint(-0.88706259976809530, 0.125580369464904700),
                IntegrationPoint(-0.73015200557404940, 0.186290210927734430),
                IntegrationPoint(-0.51909612920681180, 0.233193764591990680),
                IntegrationPoint(-0.26954315595234496, 0.262804544510246760),
                IntegrationPoint( 0.00000000000000000, 0.272925086777900900),
                IntegrationPoint( 0.26954315595234496, 0.262804544510246760),
                IntegrationPoint( 0.51909612920681180, 0.233193764591990680),
                IntegrationPoint( 0.73015200557404940, 0.186290210927734430),
                IntegrationPoint( 0.88706259976809530, 0.125580369464904700),
                IntegrationPoint( 0.97822865814605700, 0.055668567116173164)
            };
        case 12:
            return {
                IntegrationPoint(-0.98156063424671920, 0.04717533638651202),
                IntegrationPoint(-0.90411725637047480, 0.10693932599531888),
                IntegrationPoint(-0.76990267419430470, 0.16007832854334610),
                IntegrationPoint(-0.58731795428661750, 0.20316742672306565),
                IntegrationPoint(-0.36783149899818020, 0.23349253653835464),
                IntegrationPoint(-0.12523340851146890, 0.24914704581340270),
                IntegrationPoint( 0.12523340851146890, 0.24914704581340270),
                IntegrationPoint( 0.36783149899818020, 0.23349253653835464),
                IntegrationPoint( 0.58731795428661750, 0.20316742672306565),
                IntegrationPoint( 0.76990267419430470, 0.16007832854334610),
                IntegrationPoint( 0.90411725637047480, 0.10693932599531888),
                IntegrationPoint( 0.98156063424671920, 0.04717533638651202)
            };
        default:
            throw std::runtime_error("Degree not supported");
    }
}

}