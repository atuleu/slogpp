#pragma once

#include <GL/glew.h>

#include <GL/gl.h>

#include <Eigen/Core>
#include <apollo/utils/Defer.hpp>

namespace fort {
namespace apollo {

class VertexBufferBase {
public:
	virtual ~VertexBufferBase();
	virtual void Render(GLenum type) const = 0;
};

template <int VertexSize = 3, int TexelSize = 2, int ColorSize = 3>
class VertexBufferObject : public VertexBufferBase {
public:
	const static int ColumnSize = VertexSize + TexelSize + ColorSize;
	typedef Eigen::Matrix<float, Eigen::Dynamic, ColumnSize, Eigen::RowMajor>
	    Matrix;

	VertexBufferObject() {
		init();
	}

	VertexBufferObject(const Matrix &data) {
		init();
		Upload(data, true);
	}

	~VertexBufferObject() override {
		glDeleteBuffers(1, &d_ID);
	};

	void Upload(const Matrix &data, bool staticUpload = false) {
		glBindBuffer(GL_ARRAY_BUFFER, d_ID);

		defer {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.rows() * ColumnSize,
		             data.data(),
		             staticUpload ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void Render(GLenum type) const {}

private:
	GLuint d_ID;

	void init() {
		glGenBuffers(1, &d_ID);
	}
};

} // namespace apollo
} // namespace fort
