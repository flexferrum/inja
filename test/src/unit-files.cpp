#include "catch/catch.hpp"
#include "nlohmann/json.hpp"
#include "inja.hpp"


using json = nlohmann::json;


TEST_CASE("loading") {
	inja::Environment env = inja::Environment();
	json data;
	data["name"] = "Jeff";

	SECTION("Files should be loaded") {
		CHECK( env.load_global_file("../test/data/simple.txt") == "Hello {{ name }}." );
	}

	SECTION("Files should be rendered") {
		CHECK( env.render_file("../test/data/simple.txt", data) == "Hello Jeff." );
	}

	SECTION("File includes should be rendered") {
		CHECK( env.render_file("../test/data/include.txt", data) == "Answer: Hello Jeff." );
	}
}

inja::TemplatesProviderPtr prepare_inmemory_files() {
    auto files = std::make_shared<inja::InMemoryTemplatesProvider>();
    
    files->add_file("simple.txt", R"(Hello {{ name }}.)");
    files->add_file("include.txt", R"(Answer: {% include "simple.txt" %})");
    
    return files;
}

TEST_CASE("loading-inmemory") {
	inja::Environment env = inja::Environment(prepare_inmemory_files());
	json data;
	data["name"] = "Jeff";

	SECTION("Files should be loaded") {
		CHECK( env.load_global_file("simple.txt") == "Hello {{ name }}." );
	}

	SECTION("Files should be rendered") {
		CHECK( env.render_file("simple.txt", data) == "Hello Jeff." );
	}

	SECTION("File includes should be rendered") {
		CHECK( env.render_file("include.txt", data) == "Answer: Hello Jeff." );
	}
}

TEST_CASE("complete-files") {
	inja::Environment env = inja::Environment("../test/data/");

	for (std::string test_name : {"simple-file", "nested", "nested-line"}) {
		SECTION(test_name) {
			CHECK( env.render_file_with_json_file(test_name + "/template.txt", test_name + "/data.json") == env.load_global_file(test_name + "/result.txt") );
		}
	}
}

TEST_CASE("global-path") {
	inja::Environment env = inja::Environment("../test/data/", "./");
	inja::Environment env_result = inja::Environment("./");
	json data;
	data["name"] = "Jeff";

	SECTION("Files should be written") {
		env.write("simple.txt", data, "global-path-result.txt");
		CHECK( env_result.load_global_file("global-path-result.txt") == "Hello Jeff." );
	}
}
