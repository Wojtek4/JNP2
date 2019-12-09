#include "poset.h"

#include <map>
#include <vector>
#include <iostream>
#include <random>


namespace {
	using std::map, std::string, std::vector, std::pair;
	using number = unsigned long;
	using poset = pair<map<string, number>, map<number, vector<number>>>;

	std::map<number, poset>& posets() {
		static map<number, poset> m;
		return m;
	}

	bool exists_edge(number id, number id1, number id2) {
		if (id1 == id2)
			return true;
		for (auto& e : posets()[id].second[id1])
			if (e == id2)
				return true;
		return false;
	}

	number random_number() {
		static std::mt19937 rng;
		static std::uniform_int_distribution<number> dist;
		return dist(rng);
	}

	std::ostream& get_cerr() {
		static std::ios_base::Init init;
		return std::cerr;
	}

	const char* wrap_null(const char* ptr) {
		if (ptr == nullptr)
			return "NULL";
		else
			return ptr;
	}

	void introduce(const char* name) {
		get_cerr() << name << "()\n";
	}
	void introduce(const char* name, number id) {
		get_cerr() << name << "(" << id << ")\n";
	}
	void introduce(const char* name, number id, const char* v) {
		get_cerr() << name << "(" << id
			<< ", \"" << wrap_null(v) << "\")\n";
	}
	void introduce(const char* name, number id, const char* v, const char* w) {
		get_cerr() << name << "(" << id
			<< ", \"" << wrap_null(v) << "\""
			<< ", \"" << wrap_null(w) << "\")\n";
	}

	std::ostream& log(const char* name) {
		return get_cerr() << name << ": ";
	}

#ifndef NDEBUG
	const bool debug = true;
#else
	const bool debug = false;
#endif
}


namespace jnp1 {
	unsigned long poset_new(void) {
		if (debug)
			introduce(__func__);

		number new_id = random_number();

		while (posets().find(new_id) != posets().end())
			new_id++;

		posets()[new_id] = poset();

		if (debug)
			log(__func__) << "poset " << new_id << " created\n";

		return new_id;
	}

	void poset_delete(unsigned long id) {
		if (debug)
			introduce(__func__, id);

		auto deleted = posets().erase(id);

		if (debug)
			log(__func__) << "poset " << id
				<< (deleted ? " deleted\n" : " does not exist\n");
	}

	size_t poset_size(unsigned long id) {
		if (debug)
			introduce(__func__, id);

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return 0;
		}

		size_t size = posets()[id].first.size();

		if (debug)
			log(__func__) << "poset " << id << " contains "
				<< size << " element(s)\n";

		return size;
	}

	bool poset_insert(unsigned long id, char const *value) {
		if (debug)
			introduce(__func__, id, value);

		if (value == nullptr) {
			if (debug)
				log(__func__) << "invalid value (NULL)\n";
			return false;
		}

		string name = value;

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return false;
		}

		bool already_exists = posets()[id].first.find(name) != posets()[id].first.end();

		if (already_exists) {
			if (debug)
				log(__func__) << "poset " << id << ", element \"" << value
					<< "\" already exists\n";
			return false;
		}

		number new_id = random_number();
		while (posets()[id].second.find(new_id) != posets()[id].second.end())
			new_id++;

		posets()[id].first[name] = new_id;
		posets()[id].second[new_id] = vector<number>();

		if (debug)
			log(__func__) << "poset " << id << ", element \"" << value
				<< "\" inserted\n";

		return true;
	}

	bool poset_remove(unsigned long id, char const *value) {
		if (debug)
			introduce(__func__, id, value);

		if (value == nullptr) {
			if (debug)
				log(__func__) << "invalid value (NULL)\n";
			return false;
		}

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return false;
		}

		string name = value;
		bool does_not_exist =
			posets()[id].first.find(name) == posets()[id].first.end();


		if (does_not_exist) {
			if (debug)
				log(__func__) << "poset " << id << ", element \"" << value
					<< "\" does not exist\n";
			return false;
		}

		number deleted_id = posets()[id].first[name];

		posets()[id].first.erase(name);
		posets()[id].second.erase(deleted_id);

		for (auto& e : posets()[id].second)
			for (size_t i = 0; i < e.second.size(); i++)
				if (e.second[i] == deleted_id) {
					std::swap(e.second[i], e.second[e.second.size() - 1]);
					e.second.pop_back();
					i--;
				}

		if (debug)
			log(__func__) << "poset " << id << ", element \"" << value
				<< "\" removed\n";
		return true;
	}

	bool poset_add(unsigned long id, char const *value1, char const *value2) {
		if (debug)
			introduce(__func__, id, value1, value2);

		if (value1 == nullptr || value2 == nullptr) {
			if (debug)
				log(__func__) << "invalid value"
					<< (value1 == nullptr ? 1 : 2) << " (NULL)\n";
			return false;
		}

		string name1 = value1, name2 = value2;

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return false;
		}

		if (posets()[id].first.find(name1) == posets()[id].first.end()
			|| posets()[id].first.find(name2) == posets()[id].first.end()) {

			if (debug)
				log(__func__) << "poset " << id << ", element \"" << value1
					<< "\" or \"" << value2 << "\" does not exist\n";

			return false;
		}

		number id1 = posets()[id].first[name1], id2 = posets()[id].first[name2];
		bool cant_add = exists_edge(id, id2, id1) || exists_edge(id, id1, id2);


		if (cant_add) {
			if (debug)
				log(__func__) << "poset " << id << ", relation (\"" << value1
					<< "\", \"" << value2 << "\") cannot be added\n";
			return false;
		}

		for (auto& e1 : posets()[id].second)
			if (exists_edge(id, e1.first, id1))
				for (auto& e2 : posets()[id].second)
					if (exists_edge(id, id2, e2.first)
						&& !exists_edge(id, e1.first, e2.first))
						e1.second.push_back(e2.first);
		if (debug)
			log(__func__) << "poset " << id << ", relation (\"" << value1
				<< "\", \"" << value2 << "\") added\n";

		return true;
	}

	bool poset_del(unsigned long id, char const *value1, char const *value2) {
		if (debug)
			introduce(__func__, id, value1, value2);

		if (value1 == nullptr || value2 == nullptr) {
			if (debug)
				log(__func__) << "invalid value"
					<< (value1 == nullptr ? 1 : 2) << " (NULL)\n";
			return false;
		}

		string name1 = value1, name2 = value2;

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return false;
		}

		if (posets()[id].first.find(name1) == posets()[id].first.end()
			|| posets()[id].first.find(name2) == posets()[id].first.end()) {

			if (debug)
				log(__func__) << "poset " << id << ", element \"" << value1
					<< "\" or \"" << value2 << "\" does not exist\n";

			return false;
		}

		number id1 = posets()[id].first[name1], id2 = posets()[id].first[name2];

		if (id1 == id2 || !exists_edge(id, id1, id2)) {
			if (debug)
				log(__func__) << "poset " << id << ", relation (\"" << value1
					<< "\", \"" << value2 << "\") cannot be deleted\n";
			return false;
		}

		for (auto &e : posets()[id].second[id1])
			if (e != id2 && exists_edge(id, e, id2)) {
				if (debug)
					log(__func__) << "poset " << id << ", relation (\"" << value1
						<< "\", \"" << value2 << "\") cannot be deleted\n";
				return false;
			}

		vector<number>& vertices = posets()[id].second[id1];
		for (number i = 0; i < vertices.size(); i++) {
			if (vertices.at(i) == id2) {
				std::swap(vertices.at(i), vertices.at(vertices.size() - 1));
				vertices.pop_back();
				if (debug)
					log(__func__) << "poset " << id << ", relation (\"" << value1
						<< "\", \"" << value2 << "\") deleted\n";
				return true;
			}
		}

		if (debug)
			log(__func__) << "fatal error\n";
		exit(1);
	}

	bool poset_test(unsigned long id, char const *value1, char const *value2) {
		if (debug)
			introduce(__func__, id, value1, value2);

		if (value1 == nullptr || value2 == nullptr) {
			if (debug)
				log(__func__) << "invalid value"
					<< (value1 == nullptr ? 1 : 2) << " (NULL)\n";
			return false;
		}

		string name1 = value1, name2 = value2;

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return false;
		}

		if (posets()[id].first.find(name1) == posets()[id].first.end()
			|| posets()[id].first.find(name2) == posets()[id].first.end()) {

			if (debug)
				log(__func__) << "poset " << id << ", element \"" << value1
					<< "\" or \"" << value2 << "\" does not exist\n";

			return false;
		}

		number id1 = posets()[id].first[name1], id2 = posets()[id].first[name2];
		bool result = exists_edge(id, id1, id2);

		if (debug)
			log(__func__) << "poset " << id << ", relation (\"" << value1
				<< "\", \"" << value2 << "\") "
				<< (result ? "exists\n" : "does not exist\n");

		return result;
	}

	void poset_clear(unsigned long id) {
		if (debug)
			introduce(__func__, id);

		if (posets().find(id) == posets().end()) {
			if (debug)
				log(__func__) << "poset " << id << " does not exist\n";
			return;
		}

		posets()[id] = poset();
		if (debug)
			log(__func__) << "poset " << id << " cleared\n";
	}
}
