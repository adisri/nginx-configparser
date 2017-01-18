#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigStatementTest, ToString) {
	NginxConfigStatement statement;
	statement.tokens_.push_back("foo");
	statement.tokens_.push_back("bar");
	EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

class NginxStringConfigTest : public ::testing::Test {
protected:
	bool ParseString(const std::string config_string) {
		std::stringstream config_stream(config_string);
		return parser_.Parse(&config_stream, &out_config_);
	}
	NginxConfigParser parser_;
	NginxConfig out_config_;
};

TEST_F(NginxStringConfigTest, SimpleConfig) {
	EXPECT_TRUE(ParseString("foo bar;"));
	EXPECT_EQ(1, out_config_.statements_.size());
	EXPECT_EQ("foo", out_config_.statements_.at(0)->tokens_.at(0));
}

TEST_F(NginxStringConfigTest, MultiStatementConfig) {
    EXPECT_TRUE(ParseString("foo bar;abc def;"));
    EXPECT_EQ(2, out_config_.statements_.size());
    EXPECT_EQ("abc", out_config_.statements_.at(1)->tokens_.at(0));
}

TEST_F(NginxStringConfigTest, InvalidConfig) {
	EXPECT_FALSE(ParseString("foo bar"));
}

TEST_F(NginxStringConfigTest, NestedConfig) {
	EXPECT_TRUE(ParseString("server { listen 80; }"));
}

TEST_F(NginxStringConfigTest, UnmatchedBeginBraceConfig) { // failed test in original
    EXPECT_FALSE(ParseString("server { listen 80; "));
}

TEST_F(NginxStringConfigTest, UnmatchedEndBraceConfig) { // failed test in original
    EXPECT_FALSE(ParseString("abc def; }"));
}


TEST_F(NginxStringConfigTest, DoublyNestedConfig) { // failed test in original
    EXPECT_TRUE(ParseString("test_nested { server { listen 80; } }"));
}

TEST_F(NginxStringConfigTest, DoublyNestedUnmatchedConfig) { // failed test in original
    EXPECT_FALSE(ParseString("test_nested { extra { server { listen 80; } }"));
}
