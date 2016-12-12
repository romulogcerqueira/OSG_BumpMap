#include <osg/Group>
#include <osg/Image>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/StateSet>
#include <osg/TexMat>
#include <osg/TexEnv>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>

#include <osgFX/BumpMapping>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

#include <osgViewer/Viewer>

#include <osgUtil/Optimizer>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

const int TEXTURE_UNIT_SHADER_DIFFUSE = 0;
const int TEXTURE_UNIT_SHADER_NORMAL = 1;
const int TEXTURE_UNIT_SHADER_SPECULAR = 2;

osg::StateSet* createShaderBumpMap() {

    osg::StateSet* temp_state = new osg::StateSet();

    osg::Program* bumpMapProgramObject = new osg::Program;

    // Set shader
    osg::Shader* bumpVertexObject = new osg::Shader(osg::Shader::VERTEX);
    osg::Shader* bumpFragmentObject = new osg::Shader(osg::Shader::FRAGMENT);

    std::string bumpVertexFileName = osgDB::findDataFile("/home/romulo/workspace/OSG_BumpMap/bump_map.vert");
    bumpVertexObject->loadShaderSourceFromFile(bumpVertexFileName.c_str());
    std::string bumpFragmentFileName = osgDB::findDataFile("/home/romulo/workspace/OSG_BumpMap/bump_map.frag");
    bumpFragmentObject->loadShaderSourceFromFile(bumpFragmentFileName.c_str());

    bumpMapProgramObject->addShader(bumpVertexObject);
    bumpMapProgramObject->addShader(bumpFragmentObject);

    temp_state->addUniform(new osg::Uniform("diffuseTexture", TEXTURE_UNIT_SHADER_DIFFUSE));
    temp_state->addUniform(new osg::Uniform("normalTexture", TEXTURE_UNIT_SHADER_NORMAL));
    temp_state->addUniform(new osg::Uniform("specularTexture", TEXTURE_UNIT_SHADER_SPECULAR));
    temp_state->setAttributeAndModes(bumpMapProgramObject, osg::StateAttribute::ON);
    temp_state->setDataVariance(osg::Object::STATIC);

    return temp_state;
}

osg::ref_ptr<osg::StateSet> insertBumpMapTexture(osg::Image *color_image, osg::Image *normal_image, osg::Image *specular_image,
        const int texture_unit_diffuse, const int texture_unit_normal, const int texture_unit_specular) {

    if (!normal_image) {
        std::cout << "NORMAL IMAGE FAIL" << std::endl;
        exit(0);
    }

    if (!color_image) {
        std::cout << "COLOR IMAGE FAIL" << std::endl;
        exit(0);
    }

    if (!specular_image) {
        std::cout << "SPECULAR IMAGE FAIL" << std::endl;
        exit(0);
    }

    osg::StateSet* bumpState = new osg::StateSet();

    // Set textures
    osg::Texture2D *normal = new osg::Texture2D();
    osg::Texture2D *color = new osg::Texture2D();
    osg::Texture2D *specular = new osg::Texture2D();

    color->setImage(color_image);
    color->setDataVariance(osg::Object::DYNAMIC);
    color->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    color->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    color->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    color->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    color->setResizeNonPowerOfTwoHint(false);
    color->setMaxAnisotropy(8.0f);

    normal->setImage(normal_image);
    normal->setDataVariance(osg::Object::DYNAMIC);
    normal->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    normal->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    normal->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    normal->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    normal->setResizeNonPowerOfTwoHint(false);
    normal->setMaxAnisotropy(8.0f);

    specular->setImage(specular_image);
    specular->setDataVariance(osg::Object::DYNAMIC);
    specular->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    specular->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    specular->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    specular->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    specular->setResizeNonPowerOfTwoHint(false);
    specular->setMaxAnisotropy(8.0f);

    bumpState->setTextureAttributeAndModes(texture_unit_diffuse, color, osg::StateAttribute::ON);
    bumpState->setTextureAttributeAndModes(texture_unit_normal, normal, osg::StateAttribute::ON);
    bumpState->setTextureAttributeAndModes(texture_unit_specular, specular, osg::StateAttribute::ON);

    return bumpState;
}

void selectTexture(int texture, std::string *normal_path, std::string *difuse_path, std::string *specular_path) {

    std::string path = "/home/romulo/workspace/OSG_BumpMap/textures/";

    std::string texture_type;
    switch (texture) {
    case 0:
        texture_type = "gray_texture";
        break;
    case 1:
        texture_type = "yellow_texture";
        break;
    case 2:
        texture_type = "sand_texture";
        break;
    case 3:
        texture_type = "wood_texture";
        break;
    case 4:
        texture_type = "red_texture";
        break;
    case 5:
        texture_type = "concrete";
        break;
    case 6:
        texture_type = "black_texture";
        break;
    case 7:
        texture_type = "corrosion_texture";
        break;
    case 8:
        texture_type = "metal_corrosion";
        break;
    case 9:
        texture_type = "pedras_texture";
        break;
    default:
        texture_type = "";
        break;
    }

    (*normal_path) = path + texture_type + "_n.jpg";
    (*difuse_path) = path + texture_type + "_d.jpg";
    (*specular_path) = path + texture_type + "_s.jpg";

}

void addGeodeTexture(osg::Group* group) {

    std::string normal_path, difuse_path, specular_path;
    selectTexture(5, &normal_path, &difuse_path, &specular_path);

    osg::ref_ptr<osg::Image> difuse_image = osgDB::readImageFile(difuse_path);
    osg::ref_ptr<osg::Image> normal_image = osgDB::readImageFile(normal_path);
    osg::ref_ptr<osg::Image> specular_image = osgDB::readImageFile(specular_path);
    osg::ref_ptr<osg::Geode> geode(new osg::Geode);
    geode->setStateSet(
            insertBumpMapTexture(difuse_image, normal_image, specular_image, TEXTURE_UNIT_SHADER_DIFFUSE, TEXTURE_UNIT_SHADER_NORMAL,
                    TEXTURE_UNIT_SHADER_SPECULAR));
    group->addChild(geode);

    // geode without texture
    group->addChild(new osg::Geode);
}

int main(int argc, char **argv) {
    osg::ref_ptr<osg::Group> bumpRoot = new osg::Group();
    bumpRoot->setStateSet(createShaderBumpMap());
    addGeodeTexture(bumpRoot);

    osg::ref_ptr<osg::Group> original_group = (osg::Group*) osgDB::readNodeFile("/home/romulo/workspace/OSG_BumpMap/bump_map_scene.osgb");
    osg::Geode* geode_original = (osg::Geode*) original_group->getChild(3);

    bumpRoot->getChild(0)->asGeode()->addDrawable(geode_original->getDrawable(0));

    osgViewer::Viewer bumpViewer;
    bumpViewer.setUpViewInWindow(0,0,600,600);
    bumpViewer.setSceneData(bumpRoot);
    bumpViewer.run();

    return 0;
}
