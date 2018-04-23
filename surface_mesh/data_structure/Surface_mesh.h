//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011 by Graphics & Geometry Group, Bielefeld University
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================


#ifndef GRAPHENE_SURFACE_MESH_H
#define GRAPHENE_SURFACE_MESH_H


//== INCLUDES =================================================================


#include <graphene/geometry/Vector.h>
#include <graphene/geometry/Geometry_representation.h>
#include <graphene/types.h>
#include <graphene/surface_mesh/data_structure/properties.h>


//== NAMESPACE ================================================================

namespace graphene {
namespace surface_mesh {


//== CLASS DEFINITION =========================================================


/// \addtogroup surface_mesh surface_mesh
/// @{

/// A halfedge data structure for polygonal meshes.
class Surface_mesh : public geometry::Geometry_representation
{

public: //------------------------------------------------------ topology types


    /// Base class for all topology types (internally it is basically an index)
    /// \sa Vertex, Halfedge, Edge, Face
    class Base_handle
    {
    public:

        /// constructor
        explicit Base_handle(int _idx=-1) : idx_(_idx) {}

        /// Get the underlying index of this handle
        int idx() const { return idx_; }

        /// reset handle to be invalid (index=-1)
        void reset() { idx_=-1; }

        /// return whether the handle is valid, i.e., the index is not equal to -1.
        bool is_valid() const { return idx_ != -1; }

        /// are two handles equal?
        bool operator==(const Base_handle& _rhs) const {
            return idx_ == _rhs.idx_;
        }

        /// are two handles different?
        bool operator!=(const Base_handle& _rhs) const {
            return idx_ != _rhs.idx_;
        }

        /// compare operator useful for sorting handles
        bool operator<(const Base_handle& _rhs) const {
            return idx_ < _rhs.idx_;
        }

    private:
        friend class Vertex_iterator;
		friend class FeatureVertex_iterator;
        friend class Halfedge_iterator;
		friend class FeatureHalfedge_iterator;
        friend class Edge_iterator;
		friend class FeatureEdge_iterator;
        friend class Face_iterator;
		friend class FeatureLine_iterator;
        friend class Surface_mesh;
        int idx_;
    };


    /// this type represents a vertex (internally it is basically an index)
    ///  \sa Halfedge, Edge, Face
    struct Vertex : public Base_handle
    {
        /// default constructor (with invalid index)
        explicit Vertex(int _idx=-1) : Base_handle(_idx) {}
        std::ostream& operator<<(std::ostream& os) const { return os << 'v' << idx(); }
    };

	/// this type represents a vertex (internally it is basically an index)
	///  \sa Halfedge, Edge, Face
	struct FeatureVertex : public Base_handle
	{
		/// default constructor (with invalid index)
		explicit FeatureVertex(int _idx = -1) : Base_handle(_idx) {}
		std::ostream& operator<<(std::ostream& os) const { return os << 'fv' << idx(); }
	};

    /// this type represents a halfedge (internally it is basically an index)
    /// \sa Vertex, Edge, Face
    struct Halfedge : public Base_handle
    {
        /// default constructor (with invalid index)
        explicit Halfedge(int _idx=-1) : Base_handle(_idx) {}
    };

	/// this type represents a halfedge (internally it is basically an index)
	/// \sa Vertex, Edge, Face
	struct FeatureHalfedge : public Base_handle
	{
		/// default constructor (with invalid index)
		explicit FeatureHalfedge(int _idx = -1) : Base_handle(_idx) {}
	};

    /// this type represents an edge (internally it is basically an index)
    /// \sa Vertex, Halfedge, Face
    struct Edge : public Base_handle
    {
        /// default constructor (with invalid index)
        explicit Edge(int _idx=-1) : Base_handle(_idx) {}
    };

	/// this type represents an edge (internally it is basically an index)
	/// \sa Vertex, Halfedge, Face
	struct FeatureEdge : public Base_handle
	{
		/// default constructor (with invalid index)
		explicit FeatureEdge(int _idx = -1) : Base_handle(_idx) {}
	};

    /// this type represents a face (internally it is basically an index)
    /// \sa Vertex, Halfedge, Edge
    struct Face : public Base_handle
    {
        /// default constructor (with invalid index)
        explicit Face(int _idx=-1) : Base_handle(_idx) {}
    };

	struct FeatureLine : public Base_handle
	{
		explicit FeatureLine(int _idx=-1) : Base_handle(_idx){}
	};

	struct EndPoint : public Base_handle
	{
		explicit EndPoint(int _idx = -1) : Base_handle(_idx){}
	};

	float average;


public: //-------------------------------------------------- connectivity types

    /// This type stores the vertex connectivity
    /// \sa Halfedge_connectivity, Face_connectivity
    struct Vertex_connectivity
    {
        /// an outgoing halfedge per vertex (it will be a boundary halfedge for boundary vertices)
        Halfedge  halfedge_;
    };
	struct FeatureVertex_connectivity
	{
		/// an outgoing halfedge per vertex (it will be a boundary halfedge for boundary vertices)
		FeatureHalfedge  halfedge_;
	};

    /// This type stores the halfedge connectivity
    /// \sa Vertex_connectivity, Face_connectivity
    struct Halfedge_connectivity
    {
        /// face incident to halfedge
        Face      face_;
        /// vertex the halfedge points to
        Vertex    vertex_;
        /// next halfedge within a face (or along a boundary)
        Halfedge  next_halfedge_;
        /// previous halfedge within a face (or along a boundary)
        Halfedge  prev_halfedge_;
    };

	struct FeatureHalfedge_connectivity
	{
		/// vertex the halfedge points to
		FeatureVertex    vertex_;
		/// next halfedge within a face (or along a boundary)
		FeatureHalfedge  next_halfedge_;
		/// previous halfedge within a face (or along a boundary)
		FeatureHalfedge  prev_halfedge_;

		FeatureLine     line_;
		
		Face     face_;
	};

    /// This type stores the face connectivity
    /// \sa Vertex_connectivity, Halfedge_connectivity
    struct Face_connectivity
    {
        /// a halfedge that is part of the face
        Halfedge  halfedge_;
    };

	struct FeatureLine_connectivity
	{
		FeatureLine parent_;
		FeatureHalfedge halfedge_;
		FeatureVertex head_;
		FeatureVertex tail_;
		FeatureVertex or_head_;
		FeatureVertex or_tail_;
		Vertex vhead_;
		Vertex vtail_;
		Scalar length_;
		int num ;
		bool is_ridge;
		int exten_;            //3为两端点都需要延伸，2为不延伸头端点，1为不延伸尾端点，0为不延伸两端
	};

	struct EndPoint_connectivity
	{
		FeatureVertex vertex_;
		Vertex mesh_vertex_;
		Face f;
		FeatureLine l;
		bool is_head;
	};

	struct Point3D
	{
	public:
		float x, y, z;
	};


	struct FaceInfo
	{
	public:
		std::vector<float> idx;
		float r, g, b;//face color.
	};

	///WireFrame information.		
	std::vector<Point> vertset;
	std::vector<int> ridges;
	std::vector<int> ravines;

	//VSA information.
	std::vector<FaceInfo> vsa_info;
public: //------------------------------------------------------ property types

    /// Vertex property of type T
    /// \sa Halfedge_property, Edge_property, Face_property
    template <class T> class Vertex_property : public Property<T>
    {
    public:

        /// default constructor
        explicit Vertex_property() {} 
        explicit Vertex_property(Property<T> p) : Property<T>(p) {}

        /// access the data stored for vertex \c v
        typename Property<T>::reference operator[](Vertex v)
        {
            return Property<T>::operator[](v.idx());
        }

        /// access the data stored for vertex \c v
        typename Property<T>::const_reference operator[](Vertex v) const
        {
            return Property<T>::operator[](v.idx());
        }
    };

	/// Vertex property of type T
	/// \sa Halfedge_property, Edge_property, Face_property
	template <class T> class FeatureVertex_property : public Property<T>
	{
	public:

		/// default constructor
		explicit FeatureVertex_property() {}
		explicit FeatureVertex_property(Property<T> p) : Property<T>(p) {}

		/// access the data stored for vertex \c v
		typename Property<T>::reference operator[](FeatureVertex v)
		{
			return Property<T>::operator[](v.idx());
		}

		/// access the data stored for vertex \c v
		typename Property<T>::const_reference operator[](FeatureVertex v) const
		{
			return Property<T>::operator[](v.idx());
		}
	};
    /// Halfedge property of type T
    /// \sa Vertex_property, Edge_property, Face_property
    template <class T> class Halfedge_property : public Property<T>
    {
    public:

        /// default constructor
        explicit Halfedge_property() {}
        explicit Halfedge_property(Property<T> p) : Property<T>(p) {}

        /// access the data stored for halfedge \c h
        typename Property<T>::reference operator[](Halfedge h)
        {
            return Property<T>::operator[](h.idx());
        }

        /// access the data stored for halfedge \c h
        typename Property<T>::const_reference operator[](Halfedge h) const
        {
            return Property<T>::operator[](h.idx());
        }
    };

	/// Halfedge property of type T
	/// \sa Vertex_property, Edge_property, Face_property
	template <class T> class FeatureHalfedge_property : public Property<T>
	{
	public:

		/// default constructor
		explicit FeatureHalfedge_property() {}
		explicit FeatureHalfedge_property(Property<T> p) : Property<T>(p) {}

		/// access the data stored for halfedge \c h
		typename Property<T>::reference operator[](FeatureHalfedge h)
		{
			return Property<T>::operator[](h.idx());
		}

		/// access the data stored for halfedge \c h
		typename Property<T>::const_reference operator[](FeatureHalfedge h) const
		{
			return Property<T>::operator[](h.idx());
		}
	};
    /// Edge property of type T
    /// \sa Vertex_property, Halfedge_property, Face_property
    template <class T> class Edge_property : public Property<T>
    {
    public:

        /// default constructor
        explicit Edge_property() {}
        explicit Edge_property(Property<T> p) : Property<T>(p) {}

        /// access the data stored for edge \c e
        typename Property<T>::reference operator[](Edge e)
        {
            return Property<T>::operator[](e.idx());
        }

        /// access the data stored for edge \c e
        typename Property<T>::const_reference operator[](Edge e) const
        {
            return Property<T>::operator[](e.idx());
        }
    };

	/// Edge property of type T
	/// \sa Vertex_property, Halfedge_property, Face_property
	template <class T> class FeatureEdge_property : public Property<T>
	{
	public:

		/// default constructor
		explicit FeatureEdge_property() {}
		explicit FeatureEdge_property(Property<T> p) : Property<T>(p) {}

		/// access the data stored for edge \c e
		typename Property<T>::reference operator[](FeatureEdge e)
		{
			return Property<T>::operator[](e.idx());
		}

		/// access the data stored for edge \c e
		typename Property<T>::const_reference operator[](FeatureEdge e) const
		{
			return Property<T>::operator[](e.idx());
		}
	};
    /// Face property of type T
    /// \sa Vertex_property, Halfedge_property, Edge_property
    template <class T> class Face_property : public Property<T>
    {
    public:

        /// default constructor
        explicit Face_property() {}
        explicit Face_property(Property<T> p) : Property<T>(p) {}

        /// access the data stored for face \c f
        typename Property<T>::reference operator[](Face f)
        {
            return Property<T>::operator[](f.idx());
        }

        /// access the data stored for face \c f
        typename Property<T>::const_reference operator[](Face f) const
        {
            return Property<T>::operator[](f.idx());
        }
    };

	template <class T> class FeatureLine_property : public Property<T>
	{
	public:

		/// default constructor
		explicit FeatureLine_property() {}
		explicit FeatureLine_property(Property<T> p) : Property<T>(p) {}

		/// access the data stored for face \c f
		typename Property<T>::reference operator[](FeatureLine l)
		{
			return Property<T>::operator[](l.idx());
		}

		/// access the data stored for face \c f
		typename Property<T>::const_reference operator[](FeatureLine l) const
		{
			return Property<T>::operator[](l.idx());
		}
	};

	template <class T> class EndPoint_property : public Property<T>
	{
	public:

		/// default constructor
		explicit EndPoint_property() {}
		explicit EndPoint_property(Property<T> p) : Property<T>(p) {}

		/// access the data stored for face \c f
		typename Property<T>::reference operator[](EndPoint v)
		{
			return Property<T>::operator[](v.idx());
		}

		/// access the data stored for face \c f
		typename Property<T>::const_reference operator[](EndPoint v) const
		{
			return Property<T>::operator[](v.idx());
		}
	};


public: //------------------------------------------------------ iterator types

    /// this class iterates linearly over all vertices
    /// \sa vertices_begin(), vertices_end()
    /// \sa Halfedge_iterator, Edge_iterator, Face_iterator
    class Vertex_iterator
    {
    public:

        /// Default constructor
        Vertex_iterator(Vertex v=Vertex(), const Surface_mesh* m=NULL) : hnd_(v), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
        }

        /// get the vertex the iterator refers to
        Vertex operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const Vertex_iterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const Vertex_iterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        Vertex_iterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        Vertex_iterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Vertex  hnd_;
        const Surface_mesh* mesh_;
    };

	class FeatureVertex_iterator
	{
	public:

		/// Default constructor
		FeatureVertex_iterator(FeatureVertex v = FeatureVertex(), const Surface_mesh* m = NULL) : hnd_(v), mesh_(m)
		{
			if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
		}

		/// get the vertex the iterator refers to
		FeatureVertex operator*()  const { return  hnd_; }

		/// are two iterators equal?
		bool operator==(const FeatureVertex_iterator& rhs) const
		{
			return (hnd_ == rhs.hnd_);
		}

		/// are two iterators different?
		bool operator!=(const FeatureVertex_iterator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment iterator
		FeatureVertex_iterator& operator++()
		{
			++hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
			return *this;
		}

		/// pre-decrement iterator
		FeatureVertex_iterator& operator--()
		{
			--hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
			return *this;
		}

	private:
		FeatureVertex  hnd_;
		const Surface_mesh* mesh_;
	};
    /// this class iterates linearly over all halfedges
    /// \sa halfedges_begin(), halfedges_end()
    /// \sa Vertex_iterator, Edge_iterator, Face_iterator
    class Halfedge_iterator
    {
    public:

        /// Default constructor
        Halfedge_iterator(Halfedge h=Halfedge(), const Surface_mesh* m=NULL) : hnd_(h), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
        }

        /// get the halfedge the iterator refers to
        Halfedge operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const Halfedge_iterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const Halfedge_iterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        Halfedge_iterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_; 
            return *this;
        }

        /// pre-decrement iterator
        Halfedge_iterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Halfedge  hnd_;
        const Surface_mesh* mesh_;
    };

	class FeatureHalfedge_iterator
	{
	public:

		/// Default constructor
		FeatureHalfedge_iterator(FeatureHalfedge h = FeatureHalfedge(), const Surface_mesh* m = NULL) : hnd_(h), mesh_(m)
		{
			if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
		}

		/// get the halfedge the iterator refers to
		FeatureHalfedge operator*()  const { return  hnd_; }

		/// are two iterators equal?
		bool operator==(const FeatureHalfedge_iterator& rhs) const
		{
			return (hnd_ == rhs.hnd_);
		}

		/// are two iterators different?
		bool operator!=(const FeatureHalfedge_iterator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment iterator
		FeatureHalfedge_iterator& operator++()
		{
			++hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
			return *this;
		}

		/// pre-decrement iterator
		FeatureHalfedge_iterator& operator--()
		{
			--hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
			return *this;
		}

	private:
		FeatureHalfedge  hnd_;
		const Surface_mesh* mesh_;
	};
    /// this class iterates linearly over all edges
    /// \sa edges_begin(), edges_end()
    /// \sa Vertex_iterator, Halfedge_iterator, Face_iterator
    class Edge_iterator
    {
    public:

        /// Default constructor
        Edge_iterator(Edge e=Edge(), const Surface_mesh* m=NULL) : hnd_(e), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
        }

        /// get the edge the iterator refers to
        Edge operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const Edge_iterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const Edge_iterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        Edge_iterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        Edge_iterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Edge  hnd_;
        const Surface_mesh* mesh_;
    };

	class FeatureEdge_iterator
	{
	public:

		/// Default constructor
		FeatureEdge_iterator(FeatureEdge e = FeatureEdge(), const Surface_mesh* m = NULL) : hnd_(e), mesh_(m)
		{
			if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
		}

		/// get the edge the iterator refers to
		FeatureEdge operator*()  const { return  hnd_; }

		/// are two iterators equal?
		bool operator==(const FeatureEdge_iterator& rhs) const
		{
			return (hnd_ == rhs.hnd_);
		}

		/// are two iterators different?
		bool operator!=(const FeatureEdge_iterator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment iterator
		FeatureEdge_iterator& operator++()
		{
			++hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
			return *this;
		}

		/// pre-decrement iterator
		FeatureEdge_iterator& operator--()
		{
			--hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
			return *this;
		}

	private:
		FeatureEdge  hnd_;
		const Surface_mesh* mesh_;
	};
    /// this class iterates linearly over all faces
    /// \sa faces_begin(), faces_end()
    /// \sa Vertex_iterator, Halfedge_iterator, Edge_iterator
    class Face_iterator
    {
    public:

        /// Default constructor
        Face_iterator(Face f=Face(), const Surface_mesh* m=NULL) : hnd_(f), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
        }

        /// get the face the iterator refers to
        Face operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const Face_iterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const Face_iterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        Face_iterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        Face_iterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Face  hnd_;
        const Surface_mesh* mesh_;
    };

	class FeatureLine_iterator
	{
	public:

		/// Default constructor
		FeatureLine_iterator(FeatureLine l = FeatureLine(), const Surface_mesh* m = NULL) : hnd_(l), mesh_(m)
		{
			if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
		}

		/// get the face the iterator refers to
		FeatureLine operator*()  const { return  hnd_; }

		/// are two iterators equal?
		bool operator==(const FeatureLine_iterator& rhs) const
		{
			return (hnd_ == rhs.hnd_);
		}

		/// are two iterators different?
		bool operator!=(const FeatureLine_iterator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment iterator
		FeatureLine_iterator& operator++()
		{
			++hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
			return *this;
		}

		/// pre-decrement iterator
		FeatureLine_iterator& operator--()
		{
			--hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
			return *this;
		}

	private:
		FeatureLine  hnd_;
		const Surface_mesh* mesh_;
	};

	class EndPoint_iterator
	{
	public:

		/// Default constructor
		EndPoint_iterator(EndPoint v = EndPoint(), const Surface_mesh* m = NULL) : hnd_(v), mesh_(m)
		{
			if (mesh_ && mesh_->garbage()) while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
		}

		/// get the face the iterator refers to
		EndPoint operator*()  const { return  hnd_; }

		/// are two iterators equal?
		bool operator==(const EndPoint_iterator& rhs) const
		{
			return (hnd_ == rhs.hnd_);
		}

		/// are two iterators different?
		bool operator!=(const EndPoint_iterator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment iterator
		EndPoint_iterator& operator++()
		{
			++hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) ++hnd_.idx_;
			return *this;
		}

		/// pre-decrement iterator
		EndPoint_iterator& operator--()
		{
			--hnd_.idx_;
			assert(mesh_);
			while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_)) --hnd_.idx_;
			return *this;
		}

	private:
		EndPoint  hnd_;
		const Surface_mesh* mesh_;
	};


public: //-------------------------- containers for C++11 range-based for loops

    /// this helper class is a container for iterating through all
    /// vertices using C++11 range-based for-loops.
    /// \sa vertices()
    class Vertex_container
    {
    public:
        Vertex_container(Vertex_iterator _begin, Vertex_iterator _end) : begin_(_begin), end_(_end) {}
        Vertex_iterator begin() const { return begin_; }
        Vertex_iterator end()   const { return end_;   }
    private:
        Vertex_iterator begin_, end_;
    };

	class FeatureVertex_container
	{
	public:
		FeatureVertex_container(FeatureVertex_iterator _begin, FeatureVertex_iterator _end) : begin_(_begin), end_(_end) {}
		FeatureVertex_iterator begin() const { return begin_; }
		FeatureVertex_iterator end()   const { return end_; }
	private:
		FeatureVertex_iterator begin_, end_;
	};

    /// this helper class is a container for iterating through all
    /// halfedge using C++11 range-based for-loops.
    /// \sa halfedges()
    class Halfedge_container
    {
    public:
        Halfedge_container(Halfedge_iterator _begin, Halfedge_iterator _end) : begin_(_begin), end_(_end) {}
        Halfedge_iterator begin() const { return begin_; }
        Halfedge_iterator end()   const { return end_;   }
    private:
        Halfedge_iterator begin_, end_;
    };

	class FeatureHalfedge_container
	{
	public:
		FeatureHalfedge_container(FeatureHalfedge_iterator _begin, FeatureHalfedge_iterator _end) : begin_(_begin), end_(_end) {}
		FeatureHalfedge_iterator begin() const { return begin_; }
		FeatureHalfedge_iterator end()   const { return end_; }
	private:
		FeatureHalfedge_iterator begin_, end_;
	};

    /// this helper class is a container for iterating through all
    /// edges using C++11 range-based for-loops.
    /// \sa edges()
    class Edge_container
    {
    public:
        Edge_container(Edge_iterator _begin, Edge_iterator _end) : begin_(_begin), end_(_end) {}
        Edge_iterator begin() const { return begin_; }
        Edge_iterator end()   const { return end_;   }
    private:
        Edge_iterator begin_, end_;
    };

	class FeatureEdge_container
	{
	public:
		FeatureEdge_container(FeatureEdge_iterator _begin, FeatureEdge_iterator _end) : begin_(_begin), end_(_end) {}
		FeatureEdge_iterator begin() const { return begin_; }
		FeatureEdge_iterator end()   const { return end_; }
	private:
		FeatureEdge_iterator begin_, end_;
	};

    /// this helper class is a container for iterating through all
    /// faces using C++11 range-based for-loops.
    /// \sa faces()
    class Face_container
    {
    public:
        Face_container(Face_iterator _begin, Face_iterator _end) : begin_(_begin), end_(_end) {}
        Face_iterator begin() const { return begin_; }
        Face_iterator end()   const { return end_;   }
    private:
        Face_iterator begin_, end_;
    };

	class FeatureLine_container
	{
	public:
		FeatureLine_container(FeatureLine_iterator _begin, FeatureLine_iterator _end) : begin_(_begin), end_(_end) {}
		FeatureLine_iterator begin() const { return begin_; }
		FeatureLine_iterator end()   const { return end_; }
	private:
		FeatureLine_iterator begin_, end_;
	};

	class EndPoint_container
	{
	public:
		EndPoint_container(EndPoint_iterator _begin, EndPoint_iterator _end) : begin_(_begin), end_(_end) {}
		EndPoint_iterator begin() const { return begin_; }
		EndPoint_iterator end()   const { return end_; }
	private:
		EndPoint_iterator begin_, end_;
	};




public: //---------------------------------------------------- circulator types

    /// this class circulates through all one-ring neighbors of a vertex.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa Halfedge_around_vertex_circulator, Face_around_vertex_circulator, vertices(Vertex)
    class Vertex_around_vertex_circulator
    {
    public:

        /// default constructor
        Vertex_around_vertex_circulator(const Surface_mesh* m=NULL, Vertex v=Vertex())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(v);
        }

        /// are two circulators equal?
        bool operator==(const Vertex_around_vertex_circulator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const Vertex_around_vertex_circulator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotate couter-clockwise)
        Vertex_around_vertex_circulator& operator++()
        {
            assert(mesh_);
            halfedge_ = mesh_->ccw_rotated_halfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotate clockwise)
        Vertex_around_vertex_circulator& operator--()
        {
            assert(mesh_);
            halfedge_ = mesh_->cw_rotated_halfedge(halfedge_);
            return *this;
        }

        /// get the vertex the circulator refers to
        Vertex operator*()  const
        {
            assert(mesh_);
            return mesh_->to_vertex(halfedge_);
        }

        /// cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        /// return current halfedge
        Halfedge halfedge() const { return halfedge_; }

        // helper for C++11 range-based for-loops
        Vertex_around_vertex_circulator& begin() { active_=!halfedge_.is_valid(); return *this; }
        // helper for C++11 range-based for-loops
        Vertex_around_vertex_circulator& end()   { active_=true;  return *this; }

    private:
        const Surface_mesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through all outgoing halfedges of a vertex.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa Vertex_around_vertex_circulator, Face_around_vertex_circulator, halfedges(Vertex)
    class Halfedge_around_vertex_circulator
    {
    public:

        /// default constructor
        Halfedge_around_vertex_circulator(const Surface_mesh* m=NULL, Vertex v=Vertex())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(v); 
        }

        /// are two circulators equal?
        bool operator==(const Halfedge_around_vertex_circulator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const Halfedge_around_vertex_circulator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotate couter-clockwise)
        Halfedge_around_vertex_circulator& operator++()
        {
            assert(mesh_);
            halfedge_ = mesh_->ccw_rotated_halfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotate clockwise)
        Halfedge_around_vertex_circulator& operator--()
        {
            assert(mesh_);
            halfedge_ = mesh_->cw_rotated_halfedge(halfedge_);
            return *this;
        }

        /// get the halfedge the circulator refers to
        Halfedge operator*() const { return halfedge_; }

        /// cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        // helper for C++11 range-based for-loops
        Halfedge_around_vertex_circulator& begin() { active_=!halfedge_.is_valid(); return *this; }
        // helper for C++11 range-based for-loops
        Halfedge_around_vertex_circulator& end()   { active_=true;  return *this; }

    private:
        const Surface_mesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through all incident faces of a vertex.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa Vertex_around_vertex_circulator, Halfedge_around_vertex_circulator, faces(Vertex)
    class Face_around_vertex_circulator
    {
    public:

        /// construct with mesh and vertex (vertex should not be isolated!)
        Face_around_vertex_circulator(const Surface_mesh* m=NULL, Vertex v=Vertex())
        : mesh_(m), active_(true)
        {
            if (mesh_)
            {
                halfedge_ = mesh_->halfedge(v);
                if (halfedge_.is_valid() && mesh_->is_boundary(halfedge_))
                    operator++();
            }
        }

        /// are two circulators equal?
        bool operator==(const Face_around_vertex_circulator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const Face_around_vertex_circulator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotates counter-clockwise)
        Face_around_vertex_circulator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            do {
                halfedge_ = mesh_->ccw_rotated_halfedge(halfedge_);
            } while (mesh_->is_boundary(halfedge_));
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotate clockwise)
        Face_around_vertex_circulator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            do
                halfedge_ = mesh_->cw_rotated_halfedge(halfedge_);
            while (mesh_->is_boundary(halfedge_));
            return *this;
        }

        /// get the face the circulator refers to
        Face operator*() const
        {
            assert(mesh_ && halfedge_.is_valid());
            return mesh_->face(halfedge_);
        }

        /// cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        // helper for C++11 range-based for-loops
        Face_around_vertex_circulator& begin() { active_=!halfedge_.is_valid(); return *this; }
        // helper for C++11 range-based for-loops
        Face_around_vertex_circulator& end()   { active_=true;  return *this; }

    private:
        const Surface_mesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through the vertices of a face.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa Halfedge_around_face_circulator, vertices(Face)
    class Vertex_around_face_circulator
    {
    public:

        /// default constructor
        Vertex_around_face_circulator(const Surface_mesh* m=NULL, Face f=Face())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(f);
        }

        /// are two circulators equal?
        bool operator==(const Vertex_around_face_circulator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const Vertex_around_face_circulator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotates counter-clockwise)
        Vertex_around_face_circulator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->next_halfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotates clockwise)
        Vertex_around_face_circulator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->prev_halfedge(halfedge_);
            return *this;
        }

        /// get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(mesh_ && halfedge_.is_valid());
            return mesh_->to_vertex(halfedge_);
        }

        // helper for C++11 range-based for-loops
        Vertex_around_face_circulator& begin() { active_=false; return *this; }
        // helper for C++11 range-based for-loops
        Vertex_around_face_circulator& end()   { active_=true;  return *this; }

    private:
        const Surface_mesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };

	class Vertex_around_line_circulator
	{
	public:

		/// default constructor
		Vertex_around_line_circulator(const Surface_mesh* m = NULL, FeatureLine l = FeatureLine())
			: mesh_(m), active_(true)
		{
			if (mesh_) halfedge_ = mesh_->halfedge(l);
		}

		/// are two circulators equal?
		bool operator==(const Vertex_around_line_circulator& rhs) const
		{
			assert(mesh_);
			return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
		}

		/// are two circulators different?
		bool operator!=(const Vertex_around_line_circulator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment (rotates counter-clockwise)
		Vertex_around_line_circulator& operator++()
		{
			assert(mesh_ && halfedge_.is_valid());
			halfedge_ = mesh_->next_halfedge(halfedge_);
			active_ = true;
			return *this;
		}

		/// pre-decrement (rotates clockwise)
		Vertex_around_line_circulator& operator--()
		{
			assert(mesh_ && halfedge_.is_valid());
			halfedge_ = mesh_->prev_halfedge(halfedge_);
			return *this;
		}

		/// get the vertex the circulator refers to
		FeatureVertex operator*() const
		{
			assert(mesh_ && halfedge_.is_valid());
			return mesh_->to_vertex(halfedge_);
		}

		// helper for C++11 range-based for-loops
		Vertex_around_line_circulator& begin() { active_ = false; return *this; }
		// helper for C++11 range-based for-loops
		Vertex_around_line_circulator& end() { active_ = true;  return *this; }

	private:
		const Surface_mesh*  mesh_;
		FeatureHalfedge  halfedge_;
		// helper for C++11 range-based for-loops
		bool active_;
	};

    /// this class circulates through all halfedges of a face.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa Vertex_around_face_circulator, halfedges(Face)
    class Halfedge_around_face_circulator
    {
    public:

        /// default constructur
        Halfedge_around_face_circulator(const Surface_mesh* m=NULL, Face f=Face())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(f);
        }

        /// are two circulators equal?
        bool operator==(const Halfedge_around_face_circulator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const Halfedge_around_face_circulator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotates counter-clockwise)
        Halfedge_around_face_circulator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->next_halfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotates clockwise)
        Halfedge_around_face_circulator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->prev_halfedge(halfedge_);
            return *this;
        }

        /// get the halfedge the circulator refers to
        Halfedge operator*() const { return halfedge_; }

        // helper for C++11 range-based for-loops
        Halfedge_around_face_circulator& begin() { active_=false; return *this; }
        // helper for C++11 range-based for-loops
        Halfedge_around_face_circulator& end()   { active_=true;  return *this; }

    private:
        const Surface_mesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };

	class Halfedge_around_line_circulator
	{
	public:

		/// default constructur
		Halfedge_around_line_circulator(const Surface_mesh* m = NULL, FeatureLine l = FeatureLine())
			: mesh_(m), active_(true)
		{
			if (mesh_) halfedge_ = mesh_->halfedge(l);
		}

		/// are two circulators equal?
		bool operator==(const Halfedge_around_line_circulator& rhs) const
		{
			assert(mesh_);
			return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
		}

		/// are two circulators different?
		bool operator!=(const Halfedge_around_line_circulator& rhs) const
		{
			return !operator==(rhs);
		}

		/// pre-increment (rotates counter-clockwise)
		Halfedge_around_line_circulator& operator++()
		{
			assert(mesh_ && halfedge_.is_valid());
			halfedge_ = mesh_->next_halfedge(halfedge_);
			active_ = true;
			return *this;
		}

		/// pre-decrement (rotates clockwise)
		Halfedge_around_line_circulator& operator--()
		{
			assert(mesh_ && halfedge_.is_valid());
			halfedge_ = mesh_->prev_halfedge(halfedge_);
			return *this;
		}

		/// get the halfedge the circulator refers to
		FeatureHalfedge operator*() const { return halfedge_; }

		// helper for C++11 range-based for-loops
		Halfedge_around_line_circulator& begin() { active_ = true; return *this; }
		// helper for C++11 range-based for-loops
		Halfedge_around_line_circulator& end() { active_ = false;  return *this; }

	private:
		const Surface_mesh*  mesh_;
		FeatureHalfedge         halfedge_;
		// helper for C++11 range-based for-loops
		bool active_;
	};


public: //-------------------------------------------- constructor / destructor

    /// \name Construct, destruct, assignment
    //@{

    /// default constructor
    Surface_mesh();

    // destructor (is virtual, since we inherit from Geometry_representation)
    virtual ~Surface_mesh();

    /// copy constructor: copies \c rhs to \c *this. performs a deep copy of all properties.
    Surface_mesh(const Surface_mesh& rhs) { operator=(rhs); }

    /// assign \c rhs to \c *this. performs a deep copy of all properties.
    Surface_mesh& operator=(const Surface_mesh& rhs);

    /// assign \c rhs to \c *this. does not copy custom properties.
    Surface_mesh& assign(const Surface_mesh& rhs);

    //@}




public: //------------------------------------------------------------- file IO

    /// \name File IO
    //@{

    /// read mesh from file \c filename. file extension determines file type.
    /// \sa write(const std::string& filename)
    bool read(const std::string& filename,const std::string& ext);

    /// write mesh to file \c filename. file extensions determines file type.
    /// \sa read(const std::string& filename)
    bool write(const std::string& filename) const;

    //@}




public: //----------------------------------------------- add new vertex / face

    /// \name Add new elements by hand
    //@{

    /// add a new vertex with position \c p
    Vertex add_vertex(const Point& p);
	/// add a new vertex with position \c p
	FeatureVertex add_feature_vertex(const Point& p);


	void exchange_feature_vertex(const FeatureVertex v, const Point p);
    /// add a new face with vertex list \c vertices
	

    /// \sa add_triangle, add_quad

    Face add_face(const std::vector<Vertex>& vertices);

	FeatureLine add_feature_line(const std::vector<FeatureVertex>& vertices,const std::vector<int>& face_id,Scalar length,bool is_ridge = true);

	FeatureHalfedge  add_feature_halfedge(FeatureVertex start, FeatureVertex end);

	void  update_featureLine(FeatureLine l,  Vertex v, bool is_head);

	EndPoint  add_end_point(FeatureLine l, FeatureHalfedge h, FeatureVertex v, Face f, bool is_head);
    /// add a new triangle connecting vertices \c v1, \c v2, \c v3
    /// \sa add_face, add_quad
    Face add_triangle(Vertex v1, Vertex v2, Vertex v3);

    /// add a new quad connecting vertices \c v1, \c v2, \c v3, \c v4
    /// \sa add_triangle, add_face
    Face add_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);

    //@}




public: //--------------------------------------------------- memory management

    /// \name Memory Management
    //@{

    /// returns number of (deleted and valid) vertices in the mesh
    unsigned int vertices_size() const { return (unsigned int) vprops_.size(); }
	/// returns number of (deleted and vilid) feature vertices in the mesh;
	unsigned int fvertices_size() const { return (unsigned int)fvprops_.size(); }     
    /// returns number of (deleted and valid)halfedge in the mesh
    unsigned int halfedges_size() const { return (unsigned int) hprops_.size(); }
	/// returns number of feature halfedge int the mesh
	unsigned int fhalfedges_size() const { return (unsigned int)fhprops_.size(); }
    /// returns number of (deleted and valid)feature edges in the mesh
    unsigned int edges_size() const { return (unsigned int) eprops_.size(); }
	/// returns number of (deleted and valid)feature edges in the mesh
	unsigned int fedges_size() const { return (unsigned int)feprops_.size(); }
    /// returns number of (deleted and valid)faces in the mesh
    unsigned int faces_size() const { return (unsigned int) fprops_.size(); }

	unsigned int flines_size() const { return (unsigned int)lprops_.size(); }

	unsigned int endpoint_size() const { return (unsigned int)epprops_.size(); }

    /// returns number of vertices in the mesh
    unsigned int n_vertices() const { return vertices_size() - deleted_vertices_; }
	/// returns number of vertices in the mesh
	unsigned int n_fvertices() const { return fvertices_size() - deleted_feature_vertices_; }
    /// returns number of halfedge in the mesh
    unsigned int n_halfedges() const { return halfedges_size() - 2*deleted_edges_; }
	/// returns number of halfedge in the mesh
	unsigned int n_fhalfedges() const { return fhalfedges_size() - 2 * deleted_feature_edges_; }
    /// returns number of edges in the mesh
    unsigned int n_edges() const { return edges_size() - deleted_edges_; }
	/// returns number of edges in the mesh
	unsigned int n_fedges() const { return fedges_size() - deleted_feature_edges_; }
    /// returns number of faces in the mesh
    unsigned int n_faces() const { return faces_size() - deleted_faces_; }

	unsigned int n_lines() const { return flines_size() - deleted_lines_; };

	unsigned int n_endpoints()  const { return endpoint_size() - deleted_end_point_; }


    /// returns true iff the mesh is empty, i.e., has no vertices
    unsigned int empty() const { return n_vertices() == 0; }


    /// clear mesh: remove all vertices, edges, faces
    void clear();

    /// remove unused memory from vectors
    void free_memory();

    /// reserve memory (mainly used in file readers)
    void reserve(unsigned int nvertices,
                 unsigned int nedges,
                 unsigned int nfaces );

	/// reserve feature data memory (mainly used in file readers)
	void reserve(unsigned int nvertices,
		unsigned int nedges);

    /// remove deleted vertices/edges/faces
    void garbage_collection();


    /// returns whether vertex \c v is deleted
    /// \sa garbage_collection()
    bool is_deleted(Vertex v) const
    {
        return vdeleted_[v];
    }

	bool is_deleted(FeatureVertex v) const
	{
		return fvdeleted_[v];
	}
    /// returns whether halfedge \c h is deleted
    /// \sa garbage_collection()
    bool is_deleted(Halfedge h) const
    {
        return edeleted_[edge(h)];
    }
	
	bool is_deleted(FeatureHalfedge h) const
	{
		return fedeleted_[edge(h)];
	}
    /// returns whether edge \c e is deleted
    /// \sa garbage_collection()
    bool is_deleted(Edge e) const
    {
        return edeleted_[e];
    }

	bool is_deleted(FeatureEdge e) const
	{
		return fedeleted_[e];
	}
    /// returns whether face \c f is deleted
    /// \sa garbage_collection()
    bool is_deleted(Face f) const
    {
        return fdeleted_[f];
    }

	bool is_deleted(FeatureLine l) const
	{
		return ldeleted_[l];
	}

	bool is_visual(FeatureLine l) const
	{
		return lvisual_[l];
	}

	bool is_deleted(EndPoint v) const
	{
		return epdeleted_[v];
	}

    /// return whether vertex \c v is valid, i.e. the index is stores it within the array bounds.
    bool is_valid(Vertex v) const
    {
        return (0 <= v.idx()) && (v.idx() < (int)vertices_size());
    }
    /// return whether halfedge \c h is valid, i.e. the index is stores it within the array bounds.
    bool is_valid(Halfedge h) const
    {
        return (0 <= h.idx()) && (h.idx() < (int)halfedges_size());
    }
    /// return whether edge \c e is valid, i.e. the index is stores it within the array bounds.
    bool is_valid(Edge e) const
    {
        return (0 <= e.idx()) && (e.idx() < (int)edges_size());
    }
    /// return whether face \c f is valid, i.e. the index is stores it within the array bounds.
    bool is_valid(Face f) const
    {
        return (0 <= f.idx()) && (f.idx() < (int)faces_size());
    }

	bool is_valid(FeatureLine l) const
	{
		return (0 <= l.idx()) && (l.idx() < (int)flines_size());
	}

	bool is_valid(EndPoint v) const
	{
		return (0 <= v.idx()) && (v.idx() < (int)flines_size());
	}

	bool is_valid(FeatureVertex v) const
	{
		return (0 <= v.idx()) && (v.idx() < (int)fvertices_size());
	}
	/// return whether halfedge \c h is valid, i.e. the index is stores it within the array bounds.
	bool is_valid(FeatureHalfedge h) const
	{
		return (0 <= h.idx()) && (h.idx() < (int)fhalfedges_size());
	}
	/// return whether edge \c e is valid, i.e. the index is stores it within the array bounds.
	bool is_valid(FeatureEdge e) const
	{
		return (0 <= e.idx()) && (e.idx() < (int)fedges_size());
	}

    //@}




public: //---------------------------------------------- low-level connectivity

    /// \name Low-level connectivity
    //@{

    /// returns an outgoing halfedge of vertex \c v.
    /// if \c v is a boundary vertex this will be a boundary halfedge.
    Halfedge halfedge(Vertex v) const
    {
        return vconn_[v].halfedge_;
    }

	FeatureHalfedge halfedge(FeatureVertex v) const
	{
		return fvconn_[v].halfedge_;
	}
    /// set the outgoing halfedge of vertex \c v to \c h
    void set_halfedge(Vertex v, Halfedge h)
    {
        vconn_[v].halfedge_ = h;
    }

	FeatureHalfedge get_halfedge(FeatureLine l) const
	{
		return flconn_[l].halfedge_;
	}

	void set_halfedge(FeatureVertex v, FeatureHalfedge h)
	{
		fvconn_[v].halfedge_ = h;
	}
    /// returns whether \c v is a boundary vertex
    bool is_boundary(Vertex v) const
    {
        Halfedge h(halfedge(v));
        return (!(h.is_valid() && face(h).is_valid()));
    }

    /// returns whether \c v is isolated, i.e., not incident to any face
    bool is_isolated(Vertex v) const
    {
        return !halfedge(v).is_valid();
    }

    /// returns whether \c v is a manifold vertex (not incident to several patches)
    bool is_manifold(Vertex v) const
    {
        // The vertex is non-manifold if more than one gap exists, i.e.
        // more than one outgoing boundary halfedge.
        int n(0);
        Halfedge_around_vertex_circulator hit = halfedges(v), hend=hit;
        if (hit) do
        {
            if (is_boundary(*hit))
                ++n;
        }
        while (++hit!=hend);
        return n<2;
    }

    /// returns the vertex the halfedge \c h points to
    Vertex to_vertex(Halfedge h) const
    {
        return hconn_[h].vertex_;
    }

	FeatureVertex to_vertex(FeatureHalfedge h) const
	{
		return fhconn_[h].vertex_;
	}
    /// returns the vertex the halfedge \c h emanates from
    Vertex from_vertex(Halfedge h) const
    {
        return to_vertex(opposite_halfedge(h));
    }

	FeatureVertex from_vertex(FeatureHalfedge h) const
	{
		return to_vertex(opposite_halfedge(h));
	}
    /// sets the vertex the halfedge \c h points to to \c v
    void set_vertex(Halfedge h, Vertex v)
    {
        hconn_[h].vertex_ = v;
    }

	/// sets the feature vertex the halfedge \c h points to to \c v
	void set_feature_v(FeatureHalfedge h,FeatureVertex v)
	{
		fhconn_[h].vertex_ = v;
	}

	FeatureHalfedge tail_halfedge(FeatureLine l);
    /// returns the face incident to halfedge \c h
    Face face(Halfedge h) const
    {
        return hconn_[h].face_;
    }

	Face face(FeatureHalfedge h) const
	{
		return fhconn_[h].face_;
	}

	FeatureLine line(FeatureHalfedge h) const
	{
		
		return fhconn_[h].line_;
	}

	void set_line(FeatureHalfedge h, FeatureLine l, int face_id)
	{
		fhconn_[h].line_ = l;
		if (face_id == -1)
		{
			fhconn_[h].face_ = Face();
		}
		else
			fhconn_[h].face_ = Face(face_id);
	}

	void set_line_head(FeatureLine l,FeatureHalfedge h, FeatureVertex v)
	{
		flconn_[l].halfedge_ = h;
		flconn_[l].head_ = v;
	}

	void set_line_tail(FeatureLine l, FeatureVertex v)
	{
		flconn_[l].tail_ = v;
	}

	void set_line_num(FeatureLine l, int num)
	{
		flconn_[l].num = num;
	}

	int get_line_num(FeatureLine l)
	{
		return flconn_[l].num;
	}
    /// sets the incident face to halfedge \c h to \c f
    void set_face(Halfedge h, Face f)
    {
        hconn_[h].face_ = f;
    }
	
	

    /// returns the next halfedge within the incident face
    Halfedge next_halfedge(Halfedge h) const
    {
        return hconn_[h].next_halfedge_;
    }

	FeatureHalfedge next_halfedge(FeatureHalfedge h) const
	{
		return fhconn_[h].next_halfedge_;
	}
    /// sets the next halfedge of \c h within the face to \c nh
    void set_next_halfedge(Halfedge h, Halfedge nh)
    {
        hconn_[h].next_halfedge_ = nh;
        hconn_[nh].prev_halfedge_ = h;
    }

	void set_next_halfedge(FeatureHalfedge h, FeatureHalfedge nh)
	{
		fhconn_[h].next_halfedge_ = nh;
		fhconn_[nh].prev_halfedge_ = h;
	}
    /// returns the previous halfedge within the incident face
    Halfedge prev_halfedge(Halfedge h) const
    {
        return hconn_[h].prev_halfedge_;
    }

	FeatureHalfedge prev_halfedge(FeatureHalfedge h) const
	{
		return fhconn_[h].prev_halfedge_;
	}
    /// returns the opposite halfedge of \c h
    Halfedge opposite_halfedge(Halfedge h) const
    {
        return Halfedge((h.idx() & 1) ? h.idx()-1 : h.idx()+1);
    }
	FeatureHalfedge opposite_halfedge(FeatureHalfedge h) const
	{
		return FeatureHalfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
	}

    /// returns the halfedge that is rotated counter-clockwise around the
    /// start vertex of \c h. it is the opposite halfedge of the previous halfedge of \c h.
    Halfedge ccw_rotated_halfedge(Halfedge h) const
    {
        return opposite_halfedge(prev_halfedge(h));
    }

	FeatureHalfedge ccw_rotated_halfedge(FeatureHalfedge h) const
	{
		return opposite_halfedge(prev_halfedge(h));
	}

    /// returns the halfedge that is rotated clockwise around the
    /// start vertex of \c h. it is the next halfedge of the opposite halfedge of \c h.
    Halfedge cw_rotated_halfedge(Halfedge h) const
    {
        return next_halfedge(opposite_halfedge(h));
    }

	FeatureHalfedge cw_rotated_halfedge(FeatureHalfedge h) const
	{
		return next_halfedge(opposite_halfedge(h));
	}

    /// return the edge that contains halfedge \c h as one of its two halfedges.
    Edge edge(Halfedge h) const
    {
        return Edge(h.idx() >> 1);
    }

	FeatureEdge edge(FeatureHalfedge h) const
	{
		return FeatureEdge(h.idx() >> 1);
	}
    /// returns whether h is a boundary halfege, i.e., if its face does not exist.
    bool is_boundary(Halfedge h) const
    {
        return !face(h).is_valid();
    }

	bool is_boundary(FeatureHalfedge h) const
	{
		return !line(h).is_valid();
	}

    /// returns the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    Halfedge halfedge(Edge e, unsigned int i) const
    {
        assert(i<=1);
        return Halfedge((e.idx() << 1) + i);
    }

	FeatureHalfedge halfedge(FeatureEdge e, unsigned int i) const
	{
		assert(i <= 1);
		return FeatureHalfedge((e.idx() << 1) + i);
	}

    /// returns the \c i'th vertex of edge \c e. \c i has to be 0 or 1.
    Vertex vertex(Edge e, unsigned int i) const
    {
        assert(i<=1);
        return to_vertex(halfedge(e, i));
    }
	FeatureVertex vertex(FeatureEdge e, unsigned int i) const
	{
		assert(i <= 1);
		return to_vertex(halfedge(e, i));
	}
    /// returns the face incident to the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    Face face(Edge e, unsigned int i) const
    {
        assert(i<=1);
        return face(halfedge(e, i));
    }

    /// returns whether \c e is a boundary edge, i.e., if one of its
    /// halfedges is a boundary halfedge.
    bool is_boundary(Edge e) const
    {
        return (is_boundary(halfedge(e, 0)) || is_boundary(halfedge(e, 1)));
    }

    /// returns a halfedge of face \c f
    Halfedge halfedge(Face f) const
    {
        return fconn_[f].halfedge_;
    }

	FeatureLine  featureline(EndPoint ev) const
	{
		return epconn_[ev].l;
	}

	FeatureLine  prefeatureline(FeatureLine l) const
	{
		return flconn_[l].parent_;
	}

	FeatureHalfedge halfedge(FeatureLine l) const
	{
		return flconn_[l].halfedge_;
	}

	FeatureVertex head(FeatureLine l) const
	{
		return flconn_[l].head_;
	}

	Vertex mesh_head(FeatureLine l) const
	{
		return flconn_[l].vhead_;
	}

	Vertex mesh_tail(FeatureLine l) const
	{
		return flconn_[l].vtail_;
	}
	
	FeatureVertex tail(FeatureLine l) const
	{
		return flconn_[l].tail_;
	}

	bool is_ridge(FeatureLine l) const
	{
		return flconn_[l].is_ridge;
	}
    /// sets the halfedge of face \c f to \c h
    void set_halfedge(Face f, Halfedge h)
    {
        fconn_[f].halfedge_ = h;
    }

	

	

	void set_lineporp(FeatureLine l, FeatureHalfedge h,FeatureVertex head,FeatureVertex tail,Scalar length,bool is_ridge)
	{
		flconn_[l].parent_ = l;
		flconn_[l].halfedge_ = h;
		flconn_[l].head_ = head;
		flconn_[l].tail_ = tail;
		flconn_[l].or_head_ = head;
		flconn_[l].or_tail_ = tail;
		flconn_[l].vhead_ = Vertex();
		flconn_[l].vtail_ = Vertex();
		flconn_[l].is_ridge = is_ridge;
		flconn_[l].length_ = length;
		flconn_[l].exten_ = 3;
	}


	void update_lineporp(FeatureLine l, Vertex v_, bool is_head)
	{
		if (is_head)
			flconn_[l].vhead_ = v_;
		else
			flconn_[l].vtail_ = v_;
	}

	

	void set_epporp(EndPoint ep, FeatureLine l, FeatureVertex v, Face f,  bool is_head)
	{
		epconn_[ep].vertex_ = v;
		epconn_[ep].f = f;
		epconn_[ep].l = l;
		epconn_[ep].mesh_vertex_ = Vertex();
		epconn_[ep].is_head = is_head;
	}

	/*void update_epporp(EndPoint ep, Vertex v, FeatureVertex v_, Face f)
	{
		epconn_[ep].vertex_ = v_;
		epconn_[ep].mesh_vertex_ = v;
		epconn_[ep].f = f;	
	}*/

	//union_find sets search function
	void set_parent(FeatureLine l, FeatureLine parent)
	{
		flconn_[l].parent_ = parent;
	}

	FeatureLine get_parent(FeatureLine l)
	{
		return flconn_[l].parent_;
	}
	FeatureVertex get_orHead(FeatureLine l)  //获取特征线端点
	{
		return flconn_[l].or_head_;
	}

	FeatureVertex get_orTail(FeatureLine l)
	{
		return flconn_[l].or_tail_;
	}

	void set_orHead(FeatureLine l, FeatureVertex v)   //调整端点
	{
		flconn_[l].or_head_ = v;
	}

	void set_orTail(FeatureLine l, FeatureVertex v)
	{
		flconn_[l].or_tail_ = v;
	}

	Scalar get_length(FeatureLine l)
	{
		return flconn_[l].length_;
	}

	void  set_exten(FeatureLine l,int exten)
	{
		flconn_[l].exten_ = exten;
	}

	int get_exten(FeatureLine l)
	{
		return flconn_[l].exten_;
	}

	FeatureLine find_parent(FeatureLine l)
	{
		FeatureLine r = l;
		while (get_parent(r) != r)
			r = get_parent(r);
		FeatureLine i = l, j;
		while (i != r)//路径压缩算法
		{
			j = get_parent(i);//在改变他的前导点时，存储他的值
			set_parent(i,r);
			i = j;//改变他的前导点为根节点
		}
		return r;

	}

	void join(FeatureLine x, FeatureLine y) 
		//组合
	{
		FeatureLine fx = find_parent(x), fy = find_parent(y);//分别记录x,y的根节点
		if (fx != fy)//如果他们的根节点相同，则说明他们不是连通图
			set_parent(fx, fy);//将x的根结点 同 相连接
	}



	//-----------------------------------------------
    /// returns whether \c f is a boundary face, i.e., it one of its edges is a boundary edge.
    bool is_boundary(Face f) const
    {
        Halfedge h  = halfedge(f);
        Halfedge hh = h;
        do
        {
            if (is_boundary(opposite_halfedge(h)))
                return true;
            h = next_halfedge(h);
        }
        while (h != hh);
        return false;
    }

    //@}

public: //--------------------------------------------------- property handling

    /// \name Property handling
    //@{

    /** add a vertex property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> Vertex_property<T> add_vertex_property(const std::string& name, const T t=T())
    {
        return Vertex_property<T>(vprops_.add<T>(name, t));
    }

	template <class T> EndPoint_property<T> add_endpoint_property(const std::string& name, const T t = T())
	{
		return EndPoint_property<T>(epprops_.add<T>(name, t));
	}
	/** add a feature vertex property of type \c T with name \c name and default value \c t.
	fails if a property named \c name exists already, since the name has to be unique.
	in this case it returns an invalid property */
	template <class T> FeatureVertex_property<T> add_feature_v_property(const std::string& name, const T t = T())
	{
		return FeatureVertex_property<T>(fvprops_.add<T>(name, t));
	}
    /** add a halfedge property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> Halfedge_property<T> add_halfedge_property(const std::string& name, const T t=T())
    {
        return Halfedge_property<T>(hprops_.add<T>(name, t));
    }
	/** add a feature halfedge property of type \c T with name \c name and default value \c t.
	fails if a property named \c name exists already, since the name has to be unique.
	in this case it returns an invalid property */
	template <class T> FeatureHalfedge_property<T> add_feature_h_property(const std::string& name, const T t = T())
	{
		return FeatureHalfedge_property<T>(fhprops_.add<T>(name, t));
	}
    /** add a edge property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> FeatureEdge_property<T> add_feature_e_property(const std::string& name, const T t=T())
    {
        return FeatureEdge_property<T>(feprops_.add<T>(name, t));
    }
	/** add a feature edge property of type \c T with name \c name and default value \c t.
	fails if a property named \c name exists already, since the name has to be unique.
	in this case it returns an invalid property */
	template <class T> Edge_property<T> add_edge_property(const std::string& name, const T t = T())
	{
		return Edge_property<T>(eprops_.add<T>(name, t));
	}
    /** add a face property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> Face_property<T> add_face_property(const std::string& name, const T t=T())
    {
        return Face_property<T>(fprops_.add<T>(name, t));
    }
	template <class T> FeatureLine_property<T> add_line_property(const std::string& name, const T t = T())
	{
		return FeatureLine_property<T>(lprops_.add<T>(name, t));
	}


    /** get the vertex property named \c name of type \c T. returns an invalid
     Vertex_property if the property does not exist or if the type does not match. */
    template <class T> Vertex_property<T> get_vertex_property(const std::string& name) const
    {
        return Vertex_property<T>(vprops_.get<T>(name));
    }

	template <class T> EndPoint_property<T> get_endpoint_property(const std::string& name) const
	{
		return EndPoint_property<T>(epprops_.get<T>(name));
	}
	/** get the feature vertex property named \c name of type \c T. returns an invalid
	Vertex_property if the property does not exist or if the type does not match. */
	template <class T> FeatureVertex_property<T> get_feature_v_property(const std::string& name) const
	{
		return FeatureVertex_property<T>(fvprops_.get<T>(name));
	}
    /** get the halfedge property named \c name of type \c T. returns an invalid
     Vertex_property if the property does not exist or if the type does not match. */
    template <class T> Halfedge_property<T> get_halfedge_property(const std::string& name) const
    {
        return Halfedge_property<T>(hprops_.get<T>(name));
    }
	/** get the feature halfedge property named \c name of type \c T. returns an invalid
	Vertex_property if the property does not exist or if the type does not match. */
	template <class T> FeatureHalfedge_property<T> get_feature_h_property(const std::string& name) const
	{
		return FeatureHalfedge_property<T>(fhprops_.get<T>(name));
	}
    /** get the edge property named \c name of type \c T. returns an invalid
     Vertex_property if the property does not exist or if the type does not match. */
    template <class T> Edge_property<T> get_edge_property(const std::string& name) const
    {
        return Edge_property<T>(eprops_.get<T>(name));
    }
	/** get the feature edge property named \c name of type \c T. returns an invalid
	Vertex_property if the property does not exist or if the type does not match. */
	template <class T> FeatureEdge_property<T> get_feature_e_property(const std::string& name) const
	{
		return FeatureEdge_property<T>(feprops_.get<T>(name));
	}
    /** get the face property named \c name of type \c T. returns an invalid
     Vertex_property if the property does not exist or if the type does not match. */
    template <class T> Face_property<T> get_face_property(const std::string& name) const
    {
        return Face_property<T>(fprops_.get<T>(name));
    }
	template <class T> FeatureLine_property<T> get_line_property(const std::string& name) const
	{
		return FeatureLine_property<T>(lprops_.get<T>(name));
	}


    /** if a vertex property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> Vertex_property<T> vertex_property(const std::string& name, const T t=T())
    {
        return Vertex_property<T>(vprops_.get_or_add<T>(name, t));
    }

	template <class T> EndPoint_property<T> endpoint_property(const std::string& name, const T t = T())
	{
		return EndPoint_property<T>(epprops_.get_or_add<T>(name, t));
	}
	/** if a feature vertex property of type \c T with name \c name exists, it is returned.
	otherwise this property is added (with default value \c t) */
	template <class T> FeatureVertex_property<T> feature_v_property(const std::string& name, const T t = T())
	{
		return FeatureVertex_property<T>(fvprops_.get_or_add<T>(name, t));
	}
    /** if a halfedge property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> Halfedge_property<T> halfedge_property(const std::string& name, const T t=T())
    {
        return Halfedge_property<T>(hprops_.get_or_add<T>(name, t));
    }
	/** if a feature halfedge property of type \c T with name \c name exists, it is returned.
	otherwise this property is added (with default value \c t) */
	template <class T> FeatureHalfedge_property<T> feature_h_property(const std::string& name, const T t = T())
	{
		return FeatureHalfedge_property<T>(fhprops_.get_or_add<T>(name, t));
	}
    /** if an edge property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> Edge_property<T> edge_property(const std::string& name, const T t=T())
    {
        return Edge_property<T>(eprops_.get_or_add<T>(name, t));
    }
	/** if an feature edge property of type \c T with name \c name exists, it is returned.
	otherwise this property is added (with default value \c t) */
	template <class T> FeatureEdge_property<T> feature_e_property(const std::string& name, const T t = T())
	{
		return FeatureEdge_property<T>(feprops_.get_or_add<T>(name, t));
	}
    /** if a face property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> Face_property<T> face_property(const std::string& name, const T t=T())
    {
        return Face_property<T>(fprops_.get_or_add<T>(name, t));
    }

	template <class T> FeatureLine_property<T> line_property(const std::string& name, const T t = T())
	{
		return FeatureLine_property<T>(lprops_.get_or_add<T>(name, t));
	}

    /// remove the vertex property \c p
    template <class T> void remove_vertex_property(Vertex_property<T>& p)
    {
        vprops_.remove(p);
    }
	/// remove the feature vertex property \c p
	template <class T> void remove_feature_v_property(FeatureVertex_property<T>& p)
	{
		fvprops_.remove(p);
	}
    /// remove the halfedge property \c p
    template <class T> void remove_feature_h_property(FeatureHalfedge_property<T>& p)
    {
        fhprops_.remove(p);
    }
	/// remove the halfedge property \c p
	template <class T> void remove_halfedge_property(Halfedge_property<T>& p)
	{
		hprops_.remove(p);
	}
    /// remove the edge property \c p
    template <class T> void remove_edge_property(Edge_property<T>& p)
    {
        eprops_.remove(p);
    }
	/// remove the edge property \c p
	template <class T> void remove_feature_e_property(FeatureEdge_property<T>& p)
	{
		feprops_.remove(p);
	}
    /// remove the face property \c p
    template <class T> void remove_face_property(Face_property<T>& p)
    {
        fprops_.remove(p);
    }
	template <class T> void remove_line_property(FeatureLine_property<T>& p)
	{
		lprops_.remove(p);
	}

	template <class T> void remove_endpoint_property(EndPoint_property<T>& p)
	{
		epprops_.remove(p);
	}

    /** get the type_info \c T of vertex property named \c. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& get_vertex_property_type(const std::string& name)
    {
        return vprops_.get_type(name);
    }
	/** get the type_info \c T of vertex property named \c. returns an typeid(void)
	if the property does not exist or if the type does not match. */
	const std::type_info& get_feature_v_property_type(const std::string& name)
	{
		return fvprops_.get_type(name);
	}
    /** get the type_info \c T of halfedge property named \c. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& get_halfedge_property_type(const std::string& name)
    {
        return hprops_.get_type(name);
    }
	/** get the type_info \c T of halfedge property named \c. returns an typeid(void)
	if the property does not exist or if the type does not match. */
	const std::type_info& get_feaure_h_property_type(const std::string& name)
	{
		return fhprops_.get_type(name);
	}
    /** get the type_info \c T of edge property named \c. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& get_edge_property_type(const std::string& name)
    {
        return eprops_.get_type(name);
    }
	/** get the type_info \c T of edge property named \c. returns an typeid(void)
	if the property does not exist or if the type does not match. */
	const std::type_info& get_feature_e_property_type(const std::string& name)
	{
		return feprops_.get_type(name);
	}
    /** get the type_info \c T of face property named \c. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& get_face_property_type(const std::string& name)
    {
        return fprops_.get_type(name);
    }
	const std::type_info& get_line_property_type(const std::string& name)
	{
		return lprops_.get_type(name);
	}

	const std::type_info& get_endpoint_property_type(const std::string& name)
	{
		return epprops_.get_type(name);
	}
    /// returns the names of all vertex properties
    std::vector<std::string> vertex_properties() const
    {
        return vprops_.properties();
    }
	/// returns the names of all vertex properties
	std::vector<std::string> feature_v_properties() const
	{
		return fvprops_.properties();
	}
    /// returns the names of all halfedge properties
    std::vector<std::string> halfedge_properties() const
    {
        return hprops_.properties();
    }
	/// returns the names of all halfedge properties
	std::vector<std::string> feature_h_properties() const
	{
		return fhprops_.properties();
	}
    /// returns the names of all edge properties
    std::vector<std::string> edge_properties() const
    {
        return eprops_.properties();
    }
	/// returns the names of all edge properties
	std::vector<std::string> feature_e_properties() const
	{
		return feprops_.properties();
	}
    /// returns the names of all face properties
    std::vector<std::string> face_properties() const
    {
        return fprops_.properties();
    }
    
	std::vector<std::string> line_properties() const
	{
		return lprops_.properties();
	}

	std::vector<std::string> endpoint_properties() const
	{
		return epprops_.properties();
	}
	/// prints the names of all properties
    void property_stats() const;

    //@}




public: //--------------------------------------------- iterators & circulators

    /// \name Iterators & Circulators
    //@{

    /// returns start iterator for vertices
    Vertex_iterator vertices_begin() const
    {
        return Vertex_iterator(Vertex(0), this);
    }

    /// returns end iterator for vertices
    Vertex_iterator vertices_end() const
    {
        return Vertex_iterator(Vertex(vertices_size()), this);
    }

    /// returns vertex container for C++11 range-based for-loops
    Vertex_container vertices() const
    {
        return Vertex_container(vertices_begin(), vertices_end());
    }

	EndPoint_iterator endpoints_begin() const
	{
		return EndPoint_iterator(EndPoint(0), this);
	}

	/// returns end iterator for vertices
	EndPoint_iterator endpoints_end() const
	{
		return EndPoint_iterator(EndPoint(endpoint_size()), this);
	}

	/// returns vertex container for C++11 range-based for-loops
	EndPoint_container endpoints() const
	{
		return EndPoint_container(endpoints_begin(), endpoints_end());
	}

	FeatureVertex_iterator fvertices_begin() const
	{
		return FeatureVertex_iterator(FeatureVertex(0), this);
	}

	/// returns end iterator for vertices
	FeatureVertex_iterator fvertices_end() const
	{
		return FeatureVertex_iterator(FeatureVertex(fvertices_size()), this);
	}

	/// returns vertex container for C++11 range-based for-loops
	FeatureVertex_container fvertices() const
	{
		return FeatureVertex_container(fvertices_begin(), fvertices_end());
	}
    /// returns start iterator for halfedges
	Halfedge_iterator halfedges_begin() const
	{
		return Halfedge_iterator(Halfedge(0), this);
	}

	FeatureHalfedge_iterator fhalfedges_begin() const
    {
        return FeatureHalfedge_iterator(FeatureHalfedge(0), this);
    }

    /// returns end iterator for halfedges
	Halfedge_iterator halfedges_end() const
	{
		return Halfedge_iterator(Halfedge(halfedges_size()), this);
	}

	FeatureHalfedge_iterator fhalfedges_end() const
    {
        return FeatureHalfedge_iterator(FeatureHalfedge(fhalfedges_size()), this);
    }

    /// returns halfedge container for C++11 range-based for-loops
	Halfedge_container halfedges() const
	{
		return Halfedge_container(halfedges_begin(), halfedges_end());
	}

	FeatureHalfedge_container fhalfedges() const
    {
        return FeatureHalfedge_container(fhalfedges_begin(), fhalfedges_end());
    }

    /// returns start iterator for edges
    Edge_iterator edges_begin() const
    {
        return Edge_iterator(Edge(0), this);
    }

	FeatureEdge_iterator fedges_begin() const
	{
		return FeatureEdge_iterator(FeatureEdge(0), this);
	}

    /// returns end iterator for edges
    Edge_iterator edges_end() const
    {
        return Edge_iterator(Edge(edges_size()), this);
    }

	FeatureEdge_iterator fedges_end() const
	{
		return FeatureEdge_iterator(FeatureEdge(fedges_size()), this);
	}
    /// returns edge container for C++11 range-based for-loops
    Edge_container edges() const
    {
        return Edge_container(edges_begin(), edges_end());
    }

	FeatureEdge_container fedges() const
	{
		return FeatureEdge_container(fedges_begin(), fedges_end());
	}
    /// returns start iterator for faces
    Face_iterator faces_begin() const
    {
        return Face_iterator(Face(0), this);
    }

	FeatureLine_iterator lines_begin() const
	{
		return FeatureLine_iterator(FeatureLine(0), this);
	}
    /// returns end iterator for faces
    Face_iterator faces_end() const
    {
        return Face_iterator(Face(faces_size()), this);
    }

	FeatureLine_iterator lines_end() const
	{
		return FeatureLine_iterator(FeatureLine(flines_size()), this);
	}

    /// returns face container for C++11 range-based for-loops
    Face_container faces() const
    {
        return Face_container(faces_begin(), faces_end());
    }

	FeatureLine_container lines() const
	{
		return FeatureLine_container(lines_begin(), lines_end());
	}

    /// returns circulator for vertices around vertex \c v
    Vertex_around_vertex_circulator vertices(Vertex v) const
    {
        return Vertex_around_vertex_circulator(this, v);
    }

    /// returns circulator for outgoing halfedges around vertex \c v
    Halfedge_around_vertex_circulator halfedges(Vertex v) const
    {
        return Halfedge_around_vertex_circulator(this, v);
    }

    /// returns circulator for faces around vertex \c v
    Face_around_vertex_circulator faces(Vertex v) const
    {
        return Face_around_vertex_circulator(this, v);
    }

    /// returns circulator for vertices of face \c f
    Vertex_around_face_circulator vertices(Face f) const
    {
        return Vertex_around_face_circulator(this, f);
    }

	Vertex_around_line_circulator vertices(FeatureLine l) const
	{
		return Vertex_around_line_circulator(this, l);
	}

	Halfedge_around_line_circulator halfedges(FeatureLine l) const
	{
		return Halfedge_around_line_circulator(this, l);
	}

    /// returns circulator for halfedges of face \c f
    Halfedge_around_face_circulator halfedges(Face f) const
    {
        return Halfedge_around_face_circulator(this, f);
    }

    //@}





public: //--------------------------------------------- higher-level operations

    /// \name Higher-level Topological Operations
    //@{

    /// returns whether the mesh a triangle mesh. this function simply tests
    /// each face, and therefore is not very efficient.
    /// \sa trianglate(), triangulate(Face)
    bool is_triangle_mesh() const;

    /// returns whether the mesh a quad mesh. this function simply tests
    /// each face, and therefore is not very efficient.
    bool is_quad_mesh() const;

    /// triangulate the entire mesh, by calling triangulate(Face) for each face.
    /// \sa trianglate(Face)
    void triangulate();

    /// triangulate the face \c f
    /// \sa trianglate()
    void triangulate(Face f);


    /// returns whether collapsing the halfedge \c h is topologically legal.
    /// \attention This function is only valid for triangle meshes.
    bool is_collapse_ok(Halfedge h);

    /** Collapse the halfedge \c h by moving its start vertex into its target
     vertex. For non-boundary halfedges this function removes one vertex, three
     edges, and two faces. For boundary halfedges it removes one vertex, two
     edges and one face.
     \attention This function is only valid for triangle meshes.
     \attention Halfedge collapses might lead to invalid faces. Call
     is_collapse_ok(Halfedge) to be sure the collapse is legal.
     \attention The removed items are only marked as deleted. You have
     to call garbage_collection() to finally remove them.
     */
    void collapse(Halfedge h);


    /** Split the face \c f by first adding point \c p to the mesh and then
     inserting edges between \c p and the vertices of \c f. For a triangle
     this is a standard one-to-three split.
     \sa split(Face, Vertex)
     */
    Vertex split(Face f, const Point& p) { Vertex v=add_vertex(p); split(f,v); return v; }

    /** Split the face \c f by inserting edges between \c p and the vertices
     of \c f. For a triangle this is a standard one-to-three split.
     \sa split(Face, const Point&)
     */
    void split(Face f, Vertex v);


    /** Split the edge \c e by first adding point \c p to the mesh and then
     connecting it to the two vertices of the adjacent triangles that are
     opposite to edge \c e.
     \attention This function is only valid for triangle meshes.
     \sa split(Edge, Vertex)
     */
    Vertex split(Edge e, const Point& p) { Vertex v=add_vertex(p); split(e,v); return v; }

    /** Split the edge \c e by connecting vertex \c v it to the two vertices
     of the adjacent triangles that are opposite to edge \c e.
     \attention This function is only valid for triangle meshes.
     \sa split(Edge, Point)
     */
    void split(Edge e, Vertex v);


    /** Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
     (v0,p) and (p,v1). Note that this function does not introduce any other
     edge or faces. It simply splits the edge. Returns halfedge that points to \c p.
     \sa insert_vertex(Edge, Vertex)
     \sa insert_vertex(Halfedge, Vertex)
     */
    Halfedge insert_vertex(Edge e, const Point& p)
    { 
        return insert_vertex(halfedge(e,0), add_vertex(p));
    }

    /** Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
     (v0,v) and (v,v1). Note that this function does not introduce any other
     edge or faces. It simply splits the edge. Returns halfedge that points to \c p.
     \sa insert_vertex(Edge, Point)
     \sa insert_vertex(Halfedge, Vertex)
     */
    Halfedge insert_vertex(Edge e, Vertex v)
    {
        return insert_vertex(halfedge(e,0), v);
    }

    /** Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
     (v0,v) and (v,v1). Note that this function does not introduce any other
     edge or faces. It simply splits the edge. Returns halfedge that points to \c p.
     \sa insert_vertex(Edge, Point)
     \sa insert_vertex(Edge, Vertex)
     */
    Halfedge insert_vertex(Halfedge h, Vertex v);

	FeatureHalfedge insert_head_vertex(FeatureLine l, FeatureVertex v,int face_id);

	FeatureHalfedge insert_tail_vertex(FeatureLine l, FeatureVertex v,int face_id);

    /// insert edge between the to-vertices v0 of h0 and v1 of h1.
    /// returns the new halfedge from v0 to v1.
    /// \attention h0 and h1 have to belong to the same face
    Halfedge insert_edge(Halfedge h0, Halfedge h1);

	FeatureVertex find_shortest_distance_point(FeatureLine l, Point v);

    /** Check whether flipping edge \c e is topologically
     \attention This function is only valid for triangle meshes.
     \sa flip(Edge)
     */
    bool is_flip_ok(Edge e) const;

    /** Flip edge \c e: Remove edge \c e and add an edge between the two vertices
     opposite to edge \c e of the two incident triangles.
     \attention This function is only valid for triangle meshes.
     \sa is_flip_ok(Edge)
     */
    void flip(Edge e);


    /** returns the valence (number of incident edges or neighboring vertices)
     of vertex \c v. */
    unsigned int valence(Vertex v) const;

    /// returns the valence of face \c f (its number of vertices)
    unsigned int valence(Face f) const;

    /// find the halfedge from start to end
    Halfedge find_halfedge(Vertex start, Vertex end) const;

	FeatureHalfedge find_halfedge(FeatureVertex start, FeatureVertex end) const;

    /// find the edge (a,b)
    Edge find_edge(Vertex a, Vertex b) const;

    /// deletes the vertex \c v from the mesh
    void delete_vertex(Vertex v);

    /// deletes the edge \c e from the mesh
    void delete_edge(Edge e);

    /// deletes the face \c f from the mesh
    void delete_face(Face f);

    //@}


public: //------------------------------------------ geometry-related functions

    /// \name Geometry-related Functions
    //@{

    /// position of a vertex (read only)
    const Point& position(Vertex v) const { return vpoint_[v]; }

    /// position of a vertex
    Point& position(Vertex v) { return vpoint_[v]; }

    /// vector of vertex positions
    std::vector<Point>& points() { return vpoint_.vector(); }

    /// compute face normals by calling compute_face_normal(Face) for each face.
    void update_face_normals();

    /// compute normal vector of face \c f.
    Normal compute_face_normal(Face f) const;

    /// compute vertex normals by calling compute_vertex_normal(Vertex) for each vertex.
    void update_vertex_normals();

    /// compute normal vector of vertex \c v.
    Normal compute_vertex_normal(Vertex v) const;

    /// compute the length of edge \c e.
    Scalar edge_length(Edge e) const;

    //@}




private: //---------------------------------------------- allocate new elements

    /// allocate a new vertex, resize vertex properties accordingly.
    Vertex new_vertex()
    {
        vprops_.push_back();
        return Vertex(vertices_size()-1);
    }

	FeatureVertex new_feature_v()
	{
		fvprops_.push_back();
		return FeatureVertex(fvertices_size() - 1);
	}

	
    /// allocate a new edge, resize edge and halfedge properties accordingly.
    Halfedge new_edge(Vertex start, Vertex end)
    {
        assert(start != end);

        eprops_.push_back();
        hprops_.push_back();
        hprops_.push_back();

        Halfedge h0(halfedges_size()-2);
        Halfedge h1(halfedges_size()-1);

        set_vertex(h0, end);
        set_vertex(h1, start);

        return h0;
    }

FeatureHalfedge new_feature_edge(FeatureVertex start, FeatureVertex end)
	{
		assert(start != end);
		feprops_.push_back();
		fhprops_.push_back();
		fhprops_.push_back();

		FeatureHalfedge h0(fhalfedges_size() - 2);
		FeatureHalfedge h1(fhalfedges_size() - 1);

		set_feature_v(h0, end);
		set_feature_v(h1, start);

		return h0;
	}


    /// allocate a new face, resize face properties accordingly.
    Face new_face()
    {
        fprops_.push_back();
        return Face(faces_size() - 1);
    }

	FeatureLine new_faeture_line()
	{
		lprops_.push_back();
		return FeatureLine(flines_size() - 1);
	}

	EndPoint new_end_point()
	{
		epprops_.push_back();
		return EndPoint(endpoint_size() - 1);
	}

private: //--------------------------------------------------- helper functions

    /** make sure that the outgoing halfedge of vertex v is a boundary halfedge
     if v is a boundary vertex. */
    void adjust_outgoing_halfedge(Vertex v);

    /// Helper for halfedge collapse
    void remove_edge(Halfedge h);

    /// Helper for halfedge collapse
    void remove_loop(Halfedge h);

    /// are there deleted vertices, edges or faces?
    bool garbage() const { return garbage_; }



private: //------------------------------------------------------- private data

    friend bool read_poly(Surface_mesh& mesh, const std::string& filename);

    Property_container vprops_;
    Property_container hprops_;
    Property_container eprops_;
    Property_container fprops_;
	
   //feature vertex,halfedge and edge
    Property_container fvprops_; 
	Property_container fhprops_;
	Property_container feprops_;
    Property_container lprops_;
	Property_container epprops_;     //end point property

    Vertex_property<Vertex_connectivity>      vconn_;
    Halfedge_property<Halfedge_connectivity>  hconn_;
    Face_property<Face_connectivity>          fconn_;
	//feature connectivity
	FeatureVertex_property<FeatureVertex_connectivity>      fvconn_;
	FeatureHalfedge_property<FeatureHalfedge_connectivity>  fhconn_;
	FeatureLine_property<FeatureLine_connectivity>         flconn_;
	EndPoint_property<EndPoint_connectivity>               epconn_;

    Vertex_property<bool>  vdeleted_;
    Edge_property<bool>    edeleted_;
    Face_property<bool>    fdeleted_;
	
	FeatureLine_property<bool>  ldeleted_;
	FeatureLine_property<bool>     lvisual_;
	FeatureVertex_property<bool>   fvdeleted_;
	FeatureEdge_property<bool>     fedeleted_; 
	EndPoint_property<bool>        epdeleted_;

    Vertex_property<Point>   vpoint_;
	FeatureVertex_property<Point>   fpoint_;    //feature point
    Vertex_property<Normal>  vnormal_;
    Face_property<Normal>    fnormal_;

    unsigned int deleted_vertices_;
    unsigned int deleted_edges_;
    unsigned int deleted_faces_;
	unsigned int deleted_feature_vertices_;
	unsigned int deleted_feature_edges_;
	unsigned int deleted_lines_;
	unsigned int deleted_end_point_;
    bool garbage_;

    // helper data for add_face()
    typedef std::pair<Halfedge, Halfedge>  NextCacheEntry;
    typedef std::vector<NextCacheEntry>    NextCache;
    std::vector<Vertex>      add_face_vertices_;
    std::vector<Halfedge>    add_face_halfedges_;
    std::vector<bool>        add_face_is_new_;
    std::vector<bool>        add_face_needs_adjust_;
    NextCache                add_face_next_cache_;
	
	std::vector<FeatureVertex>      add_feature_vertices_;
	std::vector<FeatureHalfedge>    add_feature_halfedges_;
	typedef std::pair<FeatureHalfedge, FeatureHalfedge> NextCacheEntry1;
	typedef std::vector<NextCacheEntry1> NextCache1;
	NextCache1                add_line_next_cache_;
};


//------------------------------------------------------------ output operators


inline std::ostream& operator<<(std::ostream& os, Surface_mesh::Vertex v)
{
    return (os << 'v' << v.idx());
}

inline std::ostream& operator<<(std::ostream& os, Surface_mesh::Halfedge h)
{
    return (os << 'h' << h.idx());
}

inline std::ostream& operator<<(std::ostream& os, Surface_mesh::Edge e)
{
    return (os << 'e' << e.idx());
}

inline std::ostream& operator<<(std::ostream& os, Surface_mesh::Face f)
{
    return (os << 'f' << f.idx());
}


//=============================================================================
/// @}
//=============================================================================
} // namesoace graphene
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SURFACE_MESH_H
//=============================================================================
