#include "domain2d.h"

#include <memory>
#include <stdexcept>
#include <Eigen/LU>

#include "domain1d.h"

#if _WIN32
    #include <windows.h>
    #include <GL/gl.h> 
    #include <GL/glu.h>
#else
    #include <opengl/gl.h>
    #include <opengl/glu.h>
    #define CALLBACK
#endif

namespace {

using Vector = Eigen::VectorXd;
using Matrix = Eigen::MatrixXd;

using domain2d::Point;
using domain2d::Face;
using domain2d::Faces;

class MeshBuilder {
    std::vector<Point> m_buffer;
    Faces m_faces;
    GLenum m_type;

    void addTriangle(const int &ia, const int &ib, const int &ic) {
        Face face(3);
        face[0] = m_buffer[ia];
        face[1] = m_buffer[ib];
        face[2] = m_buffer[ic];

        m_faces.push_back(face);
    }

    double crossZ(const Point &a, const Point &b) const {
        return a(0) * b(1) - a(1) * b(0);
    }

    bool tryAddQuad(const int &ia, const int &ib, const int &ic, const int &id) {
        auto a = m_buffer[ia];
        auto b = m_buffer[ib];
        auto c = m_buffer[ic];
        auto d = m_buffer[id];

        // check for convex quad

        auto ab = b - a;
        auto bc = c - b;
        auto cd = d - c;
        auto da = a - d;

        auto abc = crossZ(ab, bc);
        auto bcd = crossZ(bc, cd);
        auto cda = crossZ(cd, da);
        auto dab = crossZ(da, ab);

        if (abc * bcd < 0.0) return false;
        if (bcd * cda < 0.0) return false;
        if (cda * dab < 0.0) return false;
        
        // add quad

        Face face(4);
        face[0] = a;
        face[1] = b;
        face[2] = c;
        face[3] = d;

        m_faces.push_back(face);

        return true;
    }

    void addTriangles() {
        for (int i = 2; i < m_buffer.size(); i += 3) {
            addTriangle(i - 2, i - 1, i);
        }
    }

    bool tryAddFanQuad(int &i) {
        if (i + 2 >= m_buffer.size()) {
            return false; // not enough vertices
        }

        auto success = tryAddQuad(0, i, i + 1, i + 2);

        if (success) {
            i += 2;
        }

        return success;
    }

    bool tryAddFanTriangle(int &i) {
        if (i + 1 >= m_buffer.size()) {
            return false; // not enough vertices
        }

        addTriangle(0, i, i + 1);

        i += 1;

        return true;
    }

    void addTriangleFan() {
        int i = 1;

        while (true) {
            if (tryAddFanQuad(i)) {
                continue;
            }

            if (tryAddFanTriangle(i)) {
                continue;
            }

            break;
        }
    }

    bool tryAddStripQuad(int &i) {
        if (i + 3 >= m_buffer.size()) {
            return false; // not enough vertices
        }

        auto success = tryAddQuad(i, i + 1, i + 3, i + 2);

        if (success) {
            i += 2;
        }

        return success;
    }

    bool tryAddStripTriangle(int &i) {
        if (i + 2 >= m_buffer.size()) {
            return false; // not enough vertices
        }
        
        addTriangle(i, i + 1, i + 2);
        
        i += 1;

        return true;
    }

    void addTriangleStrip() {
        int i = 0;

        while (true) {
            if (tryAddStripQuad(i)) {
                continue;
            }

            if (tryAddStripTriangle(i)) {
                continue;
            }

            break;
        }
    }

public:
    Faces faces() {
        return m_faces;
    }

    void begin(const GLenum &type) {
        m_type = type;
    }

    void vertex(const Point &vertex) {
        if (m_buffer.size() > 0 && m_buffer.back() == vertex) {
            return;
        }

        m_buffer.push_back(vertex);
    }

    void end() {
        switch (m_type) {
            case 4:
                addTriangles();
                break;
            case 5:
                addTriangleStrip();
                break;
            case 6:
                addTriangleFan();
                break;
            default:
                throw std::runtime_error("Unsupported OpenGL primitive (" + std::to_string(m_type) + ")");
        }

        m_buffer.clear();
    }
};

void CALLBACK onTessBeginData(GLenum type, void *polygonData) {
    auto builder = (MeshBuilder *)(polygonData);

    builder->begin(type);
}

void CALLBACK onTessVertexData(void *vertexData, void *polygonData) {
    auto builder = (MeshBuilder *)(polygonData);

    builder->vertex(*(Point *)vertexData);
}

void CALLBACK onTessEndData(void *polygonData) {
    auto builder = (MeshBuilder *)(polygonData);
    
    builder->end();
}

}

namespace domain2d {

Faces tessellate(const Paths &paths) {
    auto tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (GLvoid (CALLBACK *)())onTessBeginData);
    gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (GLvoid (CALLBACK *)())onTessVertexData);
    gluTessCallback(tess, GLU_TESS_END_DATA, (GLvoid (CALLBACK *)())onTessEndData);
    
    MeshBuilder builder;
    
    gluTessBeginPolygon(tess, &builder);

    std::vector<std::unique_ptr<Point>> vertices;

    for (const auto &path : paths) {
        gluTessBeginContour(tess);
        
        for (const auto &point : path) {
            vertices.push_back(std::make_unique<Point>(point));

            GLdouble pt[2] {point[0], point[1]};
            
            gluTessVertex(tess, pt, (void *)(vertices.back().get()));
        }

        gluTessEndContour(tess);
    }

    gluTessEndPolygon(tess);

    gluDeleteTess(tess);

    return builder.faces();
}

const IntegrationPoints normTrianglePoints(const int &degree) {
    // source: http://math2.uncc.edu/~shaodeng/TEACHING/math5172/MatlabCodes/TriGaussQuad/TriGaussPoints.m
    
    using Eigen::Vector2d;

    switch (degree) {
        case 1:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), 1.00000000000000)
            };
        case 2:
            return {
                IntegrationPoint(Vector2d(0.16666666666667, 0.16666666666667), 0.33333333333333),
                IntegrationPoint(Vector2d(0.16666666666667, 0.66666666666667), 0.33333333333333),
                IntegrationPoint(Vector2d(0.66666666666667, 0.16666666666667), 0.33333333333333)
            };
        case 3:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), -0.56250000000000),
                IntegrationPoint(Vector2d(0.20000000000000, 0.20000000000000), 0.52083333333333),
                IntegrationPoint(Vector2d(0.20000000000000, 0.60000000000000), 0.52083333333333),
                IntegrationPoint(Vector2d(0.60000000000000, 0.20000000000000), 0.52083333333333)
            };
        case 4:
            return {
                IntegrationPoint(Vector2d(0.44594849091597, 0.44594849091597), 0.22338158967801),
                IntegrationPoint(Vector2d(0.44594849091597, 0.10810301816807), 0.22338158967801),
                IntegrationPoint(Vector2d(0.10810301816807, 0.44594849091597), 0.22338158967801),
                IntegrationPoint(Vector2d(0.09157621350977, 0.09157621350977), 0.10995174365532),
                IntegrationPoint(Vector2d(0.09157621350977, 0.81684757298046), 0.10995174365532),
                IntegrationPoint(Vector2d(0.81684757298046, 0.09157621350977), 0.10995174365532)
            };
        case 5:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), 0.22500000000000),
                IntegrationPoint(Vector2d(0.47014206410511, 0.47014206410511), 0.13239415278851),
                IntegrationPoint(Vector2d(0.47014206410511, 0.05971587178977), 0.13239415278851),
                IntegrationPoint(Vector2d(0.05971587178977, 0.47014206410511), 0.13239415278851),
                IntegrationPoint(Vector2d(0.10128650732346, 0.10128650732346), 0.12593918054483),
                IntegrationPoint(Vector2d(0.10128650732346, 0.79742698535309), 0.12593918054483),
                IntegrationPoint(Vector2d(0.79742698535309, 0.10128650732346), 0.12593918054483)
            };
        case 6:
            return {
                IntegrationPoint(Vector2d(0.24928674517091, 0.24928674517091), 0.11678627572638),
                IntegrationPoint(Vector2d(0.24928674517091, 0.50142650965818), 0.11678627572638),
                IntegrationPoint(Vector2d(0.50142650965818, 0.24928674517091), 0.11678627572638),
                IntegrationPoint(Vector2d(0.06308901449150, 0.06308901449150), 0.05084490637021),
                IntegrationPoint(Vector2d(0.06308901449150, 0.87382197101700), 0.05084490637021),
                IntegrationPoint(Vector2d(0.87382197101700, 0.06308901449150), 0.05084490637021),
                IntegrationPoint(Vector2d(0.31035245103378, 0.63650249912140), 0.08285107561837),
                IntegrationPoint(Vector2d(0.63650249912140, 0.05314504984482), 0.08285107561837),
                IntegrationPoint(Vector2d(0.05314504984482, 0.31035245103378), 0.08285107561837),
                IntegrationPoint(Vector2d(0.63650249912140, 0.31035245103378), 0.08285107561837),
                IntegrationPoint(Vector2d(0.31035245103378, 0.05314504984482), 0.08285107561837),
                IntegrationPoint(Vector2d(0.05314504984482, 0.63650249912140), 0.08285107561837)
            };
        case 7:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), -0.14957004446768),
                IntegrationPoint(Vector2d(0.26034596607904, 0.26034596607904), 0.17561525743321),
                IntegrationPoint(Vector2d(0.26034596607904, 0.47930806784192), 0.17561525743321),
                IntegrationPoint(Vector2d(0.47930806784192, 0.26034596607904), 0.17561525743321),
                IntegrationPoint(Vector2d(0.06513010290222, 0.06513010290222), 0.05334723560884),
                IntegrationPoint(Vector2d(0.06513010290222, 0.86973979419557), 0.05334723560884),
                IntegrationPoint(Vector2d(0.86973979419557, 0.06513010290222), 0.05334723560884),
                IntegrationPoint(Vector2d(0.31286549600487, 0.63844418856981), 0.07711376089026),
                IntegrationPoint(Vector2d(0.63844418856981, 0.04869031542532), 0.07711376089026),
                IntegrationPoint(Vector2d(0.04869031542532, 0.31286549600487), 0.07711376089026),
                IntegrationPoint(Vector2d(0.63844418856981, 0.31286549600487), 0.07711376089026),
                IntegrationPoint(Vector2d(0.31286549600487, 0.04869031542532), 0.07711376089026),
                IntegrationPoint(Vector2d(0.04869031542532, 0.63844418856981), 0.07711376089026)
            };
        case 8:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), 0.14431560767779),
                IntegrationPoint(Vector2d(0.45929258829272, 0.45929258829272), 0.09509163426728),
                IntegrationPoint(Vector2d(0.45929258829272, 0.08141482341455), 0.09509163426728),
                IntegrationPoint(Vector2d(0.08141482341455, 0.45929258829272), 0.09509163426728),
                IntegrationPoint(Vector2d(0.17056930775176, 0.17056930775176), 0.10321737053472),
                IntegrationPoint(Vector2d(0.17056930775176, 0.65886138449648), 0.10321737053472),
                IntegrationPoint(Vector2d(0.65886138449648, 0.17056930775176), 0.10321737053472),
                IntegrationPoint(Vector2d(0.05054722831703, 0.05054722831703), 0.03245849762320),
                IntegrationPoint(Vector2d(0.05054722831703, 0.89890554336594), 0.03245849762320),
                IntegrationPoint(Vector2d(0.89890554336594, 0.05054722831703), 0.03245849762320),
                IntegrationPoint(Vector2d(0.26311282963464, 0.72849239295540), 0.02723031417443),
                IntegrationPoint(Vector2d(0.72849239295540, 0.00839477740996), 0.02723031417443),
                IntegrationPoint(Vector2d(0.00839477740996, 0.26311282963464), 0.02723031417443),
                IntegrationPoint(Vector2d(0.72849239295540, 0.26311282963464), 0.02723031417443),
                IntegrationPoint(Vector2d(0.26311282963464, 0.00839477740996), 0.02723031417443),
                IntegrationPoint(Vector2d(0.00839477740996, 0.72849239295540), 0.02723031417443)
            };
        case 9:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), 0.09713579628280),
                IntegrationPoint(Vector2d(0.48968251919874, 0.48968251919874), 0.03133470022714),
                IntegrationPoint(Vector2d(0.48968251919874, 0.02063496160252), 0.03133470022714),
                IntegrationPoint(Vector2d(0.02063496160252, 0.48968251919874), 0.03133470022714),
                IntegrationPoint(Vector2d(0.43708959149294, 0.43708959149294), 0.07782754100477),
                IntegrationPoint(Vector2d(0.43708959149294, 0.12582081701413), 0.07782754100477),
                IntegrationPoint(Vector2d(0.12582081701413, 0.43708959149294), 0.07782754100477),
                IntegrationPoint(Vector2d(0.18820353561903, 0.18820353561903), 0.07964773892721),
                IntegrationPoint(Vector2d(0.18820353561903, 0.62359292876193), 0.07964773892721),
                IntegrationPoint(Vector2d(0.62359292876193, 0.18820353561903), 0.07964773892721),
                IntegrationPoint(Vector2d(0.04472951339445, 0.04472951339445), 0.02557767565870),
                IntegrationPoint(Vector2d(0.04472951339445, 0.91054097321109), 0.02557767565870),
                IntegrationPoint(Vector2d(0.91054097321109, 0.04472951339445), 0.02557767565870),
                IntegrationPoint(Vector2d(0.22196298916077, 0.74119859878450), 0.04328353937729),
                IntegrationPoint(Vector2d(0.74119859878450, 0.03683841205474), 0.04328353937729),
                IntegrationPoint(Vector2d(0.03683841205474, 0.22196298916077), 0.04328353937729),
                IntegrationPoint(Vector2d(0.74119859878450, 0.22196298916077), 0.04328353937729),
                IntegrationPoint(Vector2d(0.22196298916077, 0.03683841205474), 0.04328353937729),
                IntegrationPoint(Vector2d(0.03683841205474, 0.74119859878450), 0.04328353937729)
            };
        case 10:
            return {
                IntegrationPoint(Vector2d(0.33333333333333, 0.33333333333333), 0.09081799038275),
                IntegrationPoint(Vector2d(0.48557763338366, 0.48557763338366), 0.03672595775647),
                IntegrationPoint(Vector2d(0.48557763338366, 0.02884473323269), 0.03672595775647),
                IntegrationPoint(Vector2d(0.02884473323269, 0.48557763338366), 0.03672595775647),
                IntegrationPoint(Vector2d(0.10948157548504, 0.10948157548504), 0.04532105943553),
                IntegrationPoint(Vector2d(0.10948157548504, 0.78103684902993), 0.04532105943553),
                IntegrationPoint(Vector2d(0.78103684902993, 0.10948157548504), 0.04532105943553),
                IntegrationPoint(Vector2d(0.30793983876412, 0.55035294182100), 0.07275791684542),
                IntegrationPoint(Vector2d(0.55035294182100, 0.14170721941488), 0.07275791684542),
                IntegrationPoint(Vector2d(0.14170721941488, 0.30793983876412), 0.07275791684542),
                IntegrationPoint(Vector2d(0.55035294182100, 0.30793983876412), 0.07275791684542),
                IntegrationPoint(Vector2d(0.30793983876412, 0.14170721941488), 0.07275791684542),
                IntegrationPoint(Vector2d(0.14170721941488, 0.55035294182100), 0.07275791684542),
                IntegrationPoint(Vector2d(0.24667256063990, 0.72832390459741), 0.02832724253106),
                IntegrationPoint(Vector2d(0.72832390459741, 0.02500353476269), 0.02832724253106),
                IntegrationPoint(Vector2d(0.02500353476269, 0.24667256063990), 0.02832724253106),
                IntegrationPoint(Vector2d(0.72832390459741, 0.24667256063990), 0.02832724253106),
                IntegrationPoint(Vector2d(0.24667256063990, 0.02500353476269), 0.02832724253106),
                IntegrationPoint(Vector2d(0.02500353476269, 0.72832390459741), 0.02832724253106),
                IntegrationPoint(Vector2d(0.06680325101220, 0.92365593358750), 0.00942166696373),
                IntegrationPoint(Vector2d(0.92365593358750, 0.00954081540030), 0.00942166696373),
                IntegrationPoint(Vector2d(0.00954081540030, 0.06680325101220), 0.00942166696373),
                IntegrationPoint(Vector2d(0.92365593358750, 0.06680325101220), 0.00942166696373),
                IntegrationPoint(Vector2d(0.06680325101220, 0.00954081540030), 0.00942166696373),
                IntegrationPoint(Vector2d(0.00954081540030, 0.92365593358750), 0.00942166696373)
            };
        case 11:
            return {
                IntegrationPoint(Vector2d(0.53461104827076, 0.53461104827076), 0.00092700632896),
                IntegrationPoint(Vector2d(0.53461104827076, -0.06922209654152), 0.00092700632896),
                IntegrationPoint(Vector2d(-0.06922209654152, 0.53461104827076), 0.00092700632896),
                IntegrationPoint(Vector2d(0.39896930296585, 0.39896930296585), 0.07714953491481),
                IntegrationPoint(Vector2d(0.39896930296585, 0.20206139406829), 0.07714953491481),
                IntegrationPoint(Vector2d(0.20206139406829, 0.39896930296585), 0.07714953491481),
                IntegrationPoint(Vector2d(0.20330990043128, 0.20330990043128), 0.05932297738077),
                IntegrationPoint(Vector2d(0.20330990043128, 0.59338019913744), 0.05932297738077),
                IntegrationPoint(Vector2d(0.59338019913744, 0.20330990043128), 0.05932297738077),
                IntegrationPoint(Vector2d(0.11935091228258, 0.11935091228258), 0.03618454050342),
                IntegrationPoint(Vector2d(0.11935091228258, 0.76129817543484), 0.03618454050342),
                IntegrationPoint(Vector2d(0.76129817543484, 0.11935091228258), 0.03618454050342),
                IntegrationPoint(Vector2d(0.03236494811128, 0.03236494811128), 0.01365973100268),
                IntegrationPoint(Vector2d(0.03236494811128, 0.93527010377745), 0.01365973100268),
                IntegrationPoint(Vector2d(0.93527010377745, 0.03236494811128), 0.01365973100268),
                IntegrationPoint(Vector2d(0.35662064826129, 0.59320121342821), 0.05233711196220),
                IntegrationPoint(Vector2d(0.59320121342821, 0.05017813831050), 0.05233711196220),
                IntegrationPoint(Vector2d(0.05017813831050, 0.35662064826129), 0.05233711196220),
                IntegrationPoint(Vector2d(0.59320121342821, 0.35662064826129), 0.05233711196220),
                IntegrationPoint(Vector2d(0.35662064826129, 0.05017813831050), 0.05233711196220),
                IntegrationPoint(Vector2d(0.05017813831050, 0.59320121342821), 0.05233711196220),
                IntegrationPoint(Vector2d(0.17148898030404, 0.80748900315979), 0.02070765963914),
                IntegrationPoint(Vector2d(0.80748900315979, 0.02102201653617), 0.02070765963914),
                IntegrationPoint(Vector2d(0.02102201653617, 0.17148898030404), 0.02070765963914),
                IntegrationPoint(Vector2d(0.80748900315979, 0.17148898030404), 0.02070765963914),
                IntegrationPoint(Vector2d(0.17148898030404, 0.02102201653617), 0.02070765963914),
                IntegrationPoint(Vector2d(0.02102201653617, 0.80748900315979), 0.02070765963914)
            };
        case 12:
            return {
                IntegrationPoint(Vector2d(0.48821738977381, 0.48821738977381), 0.02573106644045),
                IntegrationPoint(Vector2d(0.48821738977381, 0.02356522045239), 0.02573106644045),
                IntegrationPoint(Vector2d(0.02356522045239, 0.48821738977381), 0.02573106644045),
                IntegrationPoint(Vector2d(0.43972439229446, 0.43972439229446), 0.04369254453804),
                IntegrationPoint(Vector2d(0.43972439229446, 0.12055121541108), 0.04369254453804),
                IntegrationPoint(Vector2d(0.12055121541108, 0.43972439229446), 0.04369254453804),
                IntegrationPoint(Vector2d(0.27121038501212, 0.27121038501212), 0.06285822421789),
                IntegrationPoint(Vector2d(0.27121038501212, 0.45757922997577), 0.06285822421789),
                IntegrationPoint(Vector2d(0.45757922997577, 0.27121038501212), 0.06285822421789),
                IntegrationPoint(Vector2d(0.12757614554159, 0.12757614554159), 0.03479611293071),
                IntegrationPoint(Vector2d(0.12757614554159, 0.74484770891683), 0.03479611293071),
                IntegrationPoint(Vector2d(0.74484770891683, 0.12757614554159), 0.03479611293071),
                IntegrationPoint(Vector2d(0.02131735045321, 0.02131735045321), 0.00616626105156),
                IntegrationPoint(Vector2d(0.02131735045321, 0.95736529909358), 0.00616626105156),
                IntegrationPoint(Vector2d(0.95736529909358, 0.02131735045321), 0.00616626105156),
                IntegrationPoint(Vector2d(0.27571326968551, 0.60894323577979), 0.04037155776638),
                IntegrationPoint(Vector2d(0.60894323577979, 0.11534349453470), 0.04037155776638),
                IntegrationPoint(Vector2d(0.11534349453470, 0.27571326968551), 0.04037155776638),
                IntegrationPoint(Vector2d(0.60894323577979, 0.27571326968551), 0.04037155776638),
                IntegrationPoint(Vector2d(0.27571326968551, 0.11534349453470), 0.04037155776638),
                IntegrationPoint(Vector2d(0.11534349453470, 0.60894323577979), 0.04037155776638),
                IntegrationPoint(Vector2d(0.28132558098994, 0.69583608678780), 0.02235677320230),
                IntegrationPoint(Vector2d(0.69583608678780, 0.02283833222226), 0.02235677320230),
                IntegrationPoint(Vector2d(0.02283833222226, 0.28132558098994), 0.02235677320230),
                IntegrationPoint(Vector2d(0.69583608678780, 0.28132558098994), 0.02235677320230),
                IntegrationPoint(Vector2d(0.28132558098994, 0.02283833222226), 0.02235677320230),
                IntegrationPoint(Vector2d(0.02283833222226, 0.69583608678780), 0.02235677320230),
                IntegrationPoint(Vector2d(0.11625191590760, 0.85801403354407), 0.01731623110866),
                IntegrationPoint(Vector2d(0.85801403354407, 0.02573405054833), 0.01731623110866),
                IntegrationPoint(Vector2d(0.02573405054833, 0.11625191590760), 0.01731623110866),
                IntegrationPoint(Vector2d(0.85801403354407, 0.11625191590760), 0.01731623110866),
                IntegrationPoint(Vector2d(0.11625191590760, 0.02573405054833), 0.01731623110866),
                IntegrationPoint(Vector2d(0.02573405054833, 0.85801403354407), 0.01731623110866)
            };
        default:
            throw std::runtime_error("Degree not supported");
    }
}

IntegrationPoints pointsByTriangle(const Point &a, const Point &b, const Point &c, const int &degree) {
    Eigen::Matrix<double, 3, 2> vertices;
    vertices.row(0) = a;
    vertices.row(1) = b;
    vertices.row(2) = c;

    IntegrationPoints norm_points {normTrianglePoints(degree)};

    Eigen::Matrix<double, 2, 3> dn;
    dn << -1, 1, 0,
          -1, 0, 1;

    Eigen::Matrix2d jacobian = dn * vertices;
    double jacobianDet = jacobian.determinant();

    IntegrationPoints points;
    points.reserve(norm_points.size());
    
    for (const auto &norm_point : norm_points) {
        Point norm_uv = norm_point.first;
        double norm_weight = norm_point.second;

        Eigen::Vector3d n(1 - norm_uv[0] - norm_uv[1], norm_uv[0], norm_uv[1]);      

        Point uv = n.transpose() * vertices;
        double weight = 0.5 * jacobianDet * norm_weight;

        IntegrationPoint point(uv, weight);
        
        points.push_back(point);
    }

    return points;
}

IntegrationPoints pointsByQuad(const Point &a, const Point &b, const Point &c, const Point &d, const int &degreeU, const int &degreeV) {
    auto norm_points_u = domain1d::normPoints(degreeU);
    auto norm_points_v = domain1d::normPoints(degreeV);

    IntegrationPoints points;

    Eigen::Matrix<double, 4, 2> vertices;
    vertices.row(0) = a;
    vertices.row(1) = b;
    vertices.row(2) = c;
    vertices.row(3) = d;

    for (const auto &norm_point_u : norm_points_u) {
        for (const auto &norm_point_v : norm_points_v) {
            double norm_u = norm_point_u.first;
            double norm_v = norm_point_v.first;
            double norm_weight = norm_point_u.second * norm_point_v.second;

            Eigen::Vector4d n;
            n << 0.25 * (1 - norm_u) * (1 - norm_v),
                 0.25 * (1 + norm_u) * (1 - norm_v),
                 0.25 * (1 + norm_u) * (1 + norm_v),
                 0.25 * (1 - norm_u) * (1 + norm_v);

            Point uv = n.transpose() * vertices;

            Eigen::Matrix<double, 2, 4> dn;
            dn << 0.25 * (norm_v - 1), 0.25 * ( 1 - norm_v), 0.25 * (1 + norm_v), 0.25 * (-1 - norm_v),
                  0.25 * (norm_u - 1), 0.25 * (-1 - norm_u), 0.25 * (1 + norm_u), 0.25 * ( 1 - norm_u);
                            
            Eigen::Matrix2d jacobian {dn * vertices};
            double jacobianDet {jacobian.determinant()};

            double weight {jacobianDet * norm_weight};

            IntegrationPoint point(uv, weight);
            
            points.push_back(point);
        }
    }

    return points;
}

IntegrationPoints pointsByFaces(const Faces &faces, const int &degree) {
    IntegrationPoints integration_points;
    
    for (const auto face : faces) {
        IntegrationPoints points_face;
        
        switch (face.size()) {
            case 3:
                points_face = pointsByTriangle(face[0], face[1], face[2], degree);
                break;
            case 4:
                points_face = pointsByQuad(face[0], face[1], face[2], face[3], degree, degree);
                break;
            default:
                throw std::runtime_error("Invalid face");
        }

        integration_points.insert(integration_points.begin(), points_face.begin(), points_face.end());
    }
    
    return integration_points;
}

template<typename ReturnType>
ReturnType integrate(const Function<ReturnType> &func, const IntegrationPoints &points) {
    auto it = std::begin(points);

    Point uv {it->first};
    double weight {it->second};
    
    ReturnType result {func(uv[0], uv[1]) * weight};

    ++it;

    for (auto end = std::end(points); it != end; ++it) {
        uv = it->first;
        weight = it->second;

        result += func(uv[0], uv[1]) * weight;
    }

    return result;
}

template<typename ReturnType>
ReturnType integrate(const Function<ReturnType> &func, const Faces &faces, const int &degree)  {
    IntegrationPoints points {pointsByFaces(faces, degree)};

    return integrate(func, points);
}

template double integrate(const Function<double> &func, const IntegrationPoints &points);

template double integrate(const Function<double> &func, const Faces &faces, const int &degree);

template Vector integrate(const Function<Vector> &func, const IntegrationPoints &points);

template Vector integrate(const Function<Vector> &func, const Faces &faces, const int &degree);

template Matrix integrate(const Function<Matrix> &func, const IntegrationPoints &points);

template Matrix integrate(const Function<Matrix> &func, const Faces &faces, const int &degree);

}