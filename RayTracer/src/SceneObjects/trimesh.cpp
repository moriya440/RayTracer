#include <cmath>
#include <float.h>
#include "trimesh.h"

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt )
        return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    faces.push_back( newFace );
    scene->add(newFace);
    return true;
}

char *
Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

// Calculates and returns the normal of the triangle too.
bool TrimeshFace::intersectLocal( const ray& r, isect& i ) const
{
    // YOUR CODE HERE:
    // Add triangle intersection code here.
    // it currently ignores all triangles and just return false.
    //
    // Note that you are only intersecting a single triangle, and the vertices
    // of the triangle are supplied to you by the trimesh class.
    //
    // You should retrieve the vertices using code like this:
    //
    // const Vec3d& a = parent->vertices[ids[0]];
    // const Vec3d& b = parent->vertices[ids[1]];
    // const Vec3d& c = parent->vertices[ids[2]];

    Vec3d p = r.getPosition(); // Ray�̈ʒu�x�N�g��p���擾
    Vec3d d = r.getDirection(); // Ray�̕����x�N�g��d���擾
    Vec3d& alpha = parent->vertices[ids[0]]; // �O�p�`�̒��_alpha���擾
    Vec3d& beta = parent->vertices[ids[1]]; // �O�p�`�̒��_beta���擾
    Vec3d& gamma = parent->vertices[ids[2]]; // �O�p�`�̒��_gamma���擾
    Vec3d normal = ((beta - alpha) ^ (gamma - alpha)); // ���ʂ̖@��
    normal.normalize(); // ���K��
    double t; // Ray�̔}��ϐ�t
    /* t�����߂�v�Z�������ŏ����� */
    t = (normal * alpha - normal * p) / (normal * d);
    if (t < RAY_EPSILON)
        return false; // t��RAY_EPSILON��菬�����ꍇ�͕��ʂƌ������Ȃ�
    
    Vec3d Q = r.at(t); // ��_�̈ʒu�x�N�g��
    /* ��_���O�p�`�̓������𔻒肷�镔���������ŏ����� */
    Vec3d cross_alpha = ((beta - alpha) ^ (Q - alpha));
    Vec3d cross_beta = ((gamma - beta) ^ (Q - beta));
    Vec3d cross_gamma = ((alpha - gamma) ^ (Q - gamma));

    double a, b, c;
    a = cross_alpha * cross_beta;
    b = cross_beta * cross_gamma;
    c = cross_gamma * cross_alpha;

    if (a > 0 && b > 0 && c > 0) {
        i.obj = this;
        i.t = t;
        i.N = normal;
        return true;
    }
    else
        return false;

}


void
Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
        Vec3d a = vertices[(**fi)[0]];
        Vec3d b = vertices[(**fi)[1]];
        Vec3d c = vertices[(**fi)[2]];
        
        Vec3d faceNormal = ((b-a) ^ (c-a));
		faceNormal.normalize();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
}

