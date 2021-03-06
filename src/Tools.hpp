#ifndef SIMULATION_NORMAL_DEPTH_MAP_SRC_TOOLS_HPP_
#define SIMULATION_NORMAL_DEPTH_MAP_SRC_TOOLS_HPP_

// C++ includes
#include <vector>
#include <iostream>

// OSG includes
#include <osg/Node>
#include <osg/Geode>
#include <osg/TriangleFunctor>
#include <osg/Texture2D>

namespace normal_depth_map {

    /**
     * Triangle definition.
    */
    struct Triangle
    {
        std::vector<osg::Vec3f> data;

        Triangle()
            : data(5, osg::Vec3f(0, 0, 0)){};

        Triangle(osg::Vec3f v1, osg::Vec3f v2, osg::Vec3f v3)
            : data(5, osg::Vec3f(0, 0, 0))
        {
            setTriangle(v1, v2, v3);
        };

        void setTriangle(osg::Vec3f v1, osg::Vec3f v2, osg::Vec3f v3)
        {
            data[0] = v1;                   // vertex 1
            data[1] = v2;                   // vertex 2
            data[2] = v3;                   // vertex 3
            data[3] = (v1 + v2 + v3) / 3;   // centroid
            data[4] = (v2 - v1)^(v3 - v1);
            data[4].normalize();            // surface normal
        };

        // get the triangle data as vector of float
        std::vector<float> getAllDataAsVector()
        {
            float *array = &data[0].x();
            uint arraySize = data.size() * data[0].num_components;
            std::vector<float> output(array, array + arraySize);
            return output;
        }
    };

    /**
     * Bounding box definition.
    */
    struct BoundingBox
    {
        std::vector<osg::Vec3f> data;

        BoundingBox()
            : data(2, osg::Vec3f(0, 0, 0)){};

        BoundingBox(osg::Vec3f min, osg::Vec3f max)
            : data(2, osg::Vec3f(0, 0, 0))
        {
            data[0] = min;
            data[1] = max;
        };

        // get the triangle data as vector of float
        std::vector<float> getAllDataAsVector()
        {
            float *array = &data[0].x();
            uint arraySize = data.size() * data[0].num_components;
            std::vector<float> output(array, array + arraySize);
            return output;
        }
    };

    /**
     * Visit a node and store the triangles and bouding boxes data (in world coordinates)
     * of each 3D model.
     */
    class TrianglesVisitor : public osg::NodeVisitor
    {
      protected:
        struct WorldTriangle
        {
            std::vector<Triangle> triangles;
            osg::Matrixd local2world;

            inline void operator()(const osg::Vec3f &v1,
                                   const osg::Vec3f &v2,
                                   const osg::Vec3f &v3,
                                   bool treatVertexDataAsTemporary)
            {
                // transform vertice coordinates to world coordinates
                osg::Vec3f v1_w = v1 * local2world;
                osg::Vec3f v2_w = v2 * local2world;
                osg::Vec3f v3_w = v3 * local2world;
                triangles.push_back(Triangle(v1_w, v2_w, v3_w));
            };
        };
        osg::TriangleFunctor<WorldTriangle> tf;
        std::vector<uint> trianglesRef;
        std::vector<BoundingBox> bboxes;

      public:
        TrianglesVisitor()
        {
            setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
            trianglesRef.push_back(0);
        };

        void apply(osg::Geode &geode)
        {
            // local to world matrix
            tf.local2world = osg::computeLocalToWorld(this->getNodePath());

            for (size_t idx = 0; idx < geode.getNumDrawables(); ++idx)
            {
                // triangles
                geode.getDrawable(idx)->accept(tf);
                trianglesRef.push_back(tf.triangles.size());

                // bounding boxes
                osg::BoundingBox bb = geode.getDrawable(idx)->getBound();
                BoundingBox bb_w(bb._min * tf.local2world, bb._max * tf.local2world);
                bboxes.push_back(bb_w);
            }
        }

        std::vector<Triangle> getTriangles() { return tf.triangles; };
        std::vector<uint> getTrianglesRef() { return trianglesRef; };
        std::vector<BoundingBox> getBoundingBoxes() { return bboxes; };
    };

    /**
     * Set the pixel value of an osg image.
     *
     * @param image: source OSG image data.
     * @param x: column index of source image.
     * @param y: row index of source image.
     * @param k: channel index of source image.
     * @param value: the new pixel value.
     */
    template <typename T>
    void setOSGImagePixel(osg::ref_ptr<osg::Image> &image,
                          unsigned int x,
                          unsigned int y,
                          unsigned int k,
                          T value)
    {

        bool valid = (y < (unsigned int)image->s())
                     && (x < (unsigned int)image->t())
                     && (k < (unsigned int)image->r());

        if (!valid) {
            std::cout << "Not valid" << std::endl;
            return;
        }

        uint step = (x * image->s() + y) * image->r() + k;

        T *data = (T *)image->data();
        data = data + step;
        *data = value;
    }

    /**
     * Convert the triangles/meshes data extracted from 3D models into
     * an OSG texture to be read by shader.
     *
     * @param triangles: vector of triangles.
     * @param trianglesRef: index of triangles per 3D model on texture.
     * @param bboxes: bounding box data (vmin and vmax) of each 3D model.
     * @param texture: final OSG texture with triangles data.
     */
    void triangles2texture(
        std::vector<Triangle> triangles,
        std::vector<uint> trianglesRef,
        std::vector<BoundingBox> bboxes,
        osg::ref_ptr<osg::Texture2D> &texture);
}

#endif
