/*
 * SharedPtr.hpp
 *
 *  Created on: Nov 18, 2017
 *      Author: pragyavishalakshi
 */

#ifndef SHAREDPTR_HPP_
#define SHAREDPTR_HPP_

#include<iostream>

namespace cs540 {

class Deleter {
public:
	virtual ~Deleter() {};
};

template <typename U>
class helper:public Deleter {
public:
	U *pointer;

	helper() {
		pointer = nullptr;
	}

	helper(U* ptr) {
		pointer = ptr;
	}

	~helper() {
		delete pointer;
	}
};

class reference_count {
public:
	int counter;

public:
	void inc_ref() {
		++counter;
	}

	int dec_ref() {
		return --counter;
	}
};


template <typename T>
class SharedPtr {
public:
	reference_count *rc;
	Deleter *d;

public:
	SharedPtr(): rc{new reference_count()}, d{nullptr} {
		rc->inc_ref();
	}

	template <typename U>
	explicit SharedPtr(U *u): rc{0}, d{nullptr} {
		d = new helper<U>(u);
		rc = new reference_count();
		rc->inc_ref();
	}

	SharedPtr(const SharedPtr &p): rc{0}, d{nullptr} {
		if(p.d != NULL) {
			rc = p.rc;
			d = p.d;
			rc->inc_ref();
		}
	}

	template <typename U>
	SharedPtr(const SharedPtr<U> &p): rc{0}, d{nullptr} {
		if(p.d != NULL) {
			rc = p.rc;
			d = p.d;
			rc->inc_ref();
		}
	}

	~SharedPtr() {
		if(rc->counter > 0) {
			if(rc->dec_ref() == 0) {
				delete rc;
				if(d != nullptr) {
					delete d;
				}
			}
		}
	}

	SharedPtr(SharedPtr &&p) {
		if(this != &p) {
			if(rc->dec_ref() == 0) {
				delete rc;
				if(d != nullptr) {
					delete d;
				}
			}
		}
		rc = p.rc;
		d = p.d;
		p.rc = new reference_count();
		p.rc->inc_ref();
		p.d = nullptr;
	}

	template <typename U>
	SharedPtr(SharedPtr<U> &&p) {
		if(this != &p) {
			if(rc->dec_ref() == 0) {
				delete rc;
				if(d != nullptr) {
					delete d;
				}
			}
		}
		rc = p.rc;
		d = p.d;
		p.rc = new reference_count();
		p.rc->inc_ref();
		p.d = nullptr;
	}

	SharedPtr &operator=(const SharedPtr &p) {
		if(this != &p) {
			if(rc->dec_ref() == 0) {
				delete rc;
				if(d != nullptr) {
					delete d;
				}
			}
			rc = p.rc;
			rc->inc_ref();
			d = p.d;
		}
		return *this;
	}

	template <typename U>
	SharedPtr<T> &operator=(const SharedPtr<U> &p) {
		if(rc->dec_ref() == 0) {
			delete rc;
			if(d != nullptr) {
				delete d;
			}
		}
		rc = p.rc;
		rc->inc_ref();
		d = p.d;
		return *this;
	}

	SharedPtr &operator=(SharedPtr &&p) {
		if(this != &p) {
			if(rc->dec_ref() == 0) {
				delete rc;
				if(d != nullptr) {
					delete d;
				}
			}
		}
		rc = p.rc;
		d = p.d;
		p.rc = new reference_count();
		p.rc->inc_ref();
		p.d = nullptr;
		return *this;
	}

	template <typename U>
	SharedPtr &operator=(SharedPtr<U> &&p) {
		if(this != &p) {
			if(rc->dec_ref() == 0) {
				delete rc;
				if(d != nullptr) {
					delete d;
				}
			}
		}
		rc = p.rc;
		d = p.d;
		p.rc = new reference_count();
		p.rc->inc_ref();
		p.d = nullptr;
		return *this;
	}

	void reset() {
		if(rc->dec_ref() == 0) {
			delete rc;
			if(d != nullptr) {
				delete d;
			}
		}
		rc = new reference_count();
		rc->inc_ref();
		d = nullptr;
	}

	template <typename U>
	void reset(U *p) {
		if(rc->dec_ref() == 0) {
			delete rc;
			if(d != nullptr) {
				delete d;
			}
		}
		d = new helper<U>(p);
		rc = new reference_count();
		rc->inc_ref();
	}

	T *get() const {
		helper<T>* t = static_cast<helper<T>*>(d);
		return t->pointer;
	}

	T &operator*() const {
		helper<T>* t = static_cast<helper<T>*>(d);
		return *(t->pointer);
	}

	T *operator->() const {
		helper<T>* t = static_cast<helper<T>*>(d);
		return t->pointer;
	}

	explicit operator bool() const {
		return (d != nullptr);
	}

};

template <typename T1, typename T2>
bool operator==(const SharedPtr<T1> &s1, const SharedPtr<T2> &s2) {
	bool flag = false;
	if (s1.d == s2.d) {
		if (s1.rc == s2.rc) {
			flag = true;
		}
	}
	return flag;
}

template <typename T>
bool operator==(const SharedPtr<T> &s, std::nullptr_t) {
	return (s.d == nullptr);
}

template <typename T>
bool operator==(std::nullptr_t, const SharedPtr<T> &s) {
	return (s.d == nullptr);
}

template <typename T1, typename T2>
bool operator!=(const SharedPtr<T1> &s1, const SharedPtr<T2> &s2) {
	bool flag = false;
	if (s1.d != s2.d) {
		if (s1.rc != s2.rc) {
			flag = true;
		}
	}
	return flag;
}

template <typename T>
bool operator!=(const SharedPtr<T> &s, std::nullptr_t) {
	return (s.d != nullptr);
}

template <typename T>
bool operator!=(std::nullptr_t, const SharedPtr<T> &s) {
	return (s.d != nullptr);
}

template <typename T, typename U>
SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp) {
	SharedPtr<T> s;
	delete s.rc;
	s.d = static_cast<helper<T>*>(sp.d);
	s.rc = sp.rc;
	s.rc->inc_ref();
	return s;
}

template <typename T, typename U>
SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp) {
	SharedPtr<T> s;
	delete s.rc;
	s.d = dynamic_cast<helper<T>*>(sp.d);
	s.rc = sp.rc;
	s.rc->inc_ref();
	return s;
}

}

#endif /* SHAREDPTR_HPP_ */
