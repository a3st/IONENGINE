// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<typename T>
class sparse_set {

    static_assert(std::is_unsigned<T>::value, "sparse_set value should be only unsigned type");

public:

    using iterator_t = typename std::vector<T>::const_iterator;
	using const_iterator_t = typename std::vector<T>::const_iterator;
	
    sparse_set() : m_size(0), m_capacity(0) {
    }

	iterator_t begin() { 
        return dense.begin(); 
    }

	const_iterator_t begin() const { 
        return dense.begin(); 
    }

	iterator_t end() { 
        return dense.begin() + m_size; 
    }

	const_iterator_t end() const 	{ 
        return dense.begin() + m_size; 
    }

    void clear() { 
        m_size = 0; 
    }

    usize size() {
        return m_size;
    } 

    usize capacity() {
        return m_capacity;
    } 

    void reserve(const usize size) {
		if (size > m_capacity) {
			m_dense.resize(size, 0);
			m_sparse.resize(size, 0);
			m_capacity = size;
		}
	}

	bool has(const T& value) const {
		return value < m_capacity && m_sparse[value] < m_size && m_dense[m_sparse[value]] == value;
	}

	void insert(const T& value) {
		if (!has(value)) {
			if (value >= m_capacity) {
				reserve(value + 1);
            }
			m_dense[m_size] = value;
			m_sparse[value] = m_size;
			++m_size;
		}
	}

	void erase(const T& value) {
		if (has(value)) {
			m_dense[m_sparse[value]] = m_dense[m_size - 1];
			m_sparse[m_dense[m_size - 1]] = m_sparse[value];
			--m_size;
		}
	}

private:

    std::vector<T> m_dense;
    std::vector<T> m_sparse;

    usize m_capacity;
    usize m_size;
};

}