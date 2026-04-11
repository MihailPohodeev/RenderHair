#include "catch2/catch_test_macros.hpp"
#include "RenderHair.hpp"

TEST_CASE("Trimming checks")
{
    CHECK(RenderHair::trim("") == "");
    CHECK(RenderHair::trim(" ") == "");
    CHECK(RenderHair::trim("  ") == "");
    CHECK(RenderHair::trim(" spaces ") == "spaces");
    CHECK(RenderHair::trim("nospace") == "nospace");
    CHECK(RenderHair::trim(" leftspace") == "leftspace");
    CHECK(RenderHair::trim("rightspace ") == "rightspace");
    CHECK(RenderHair::trim("  morespaces    ") == "morespaces");
    CHECK(RenderHair::trim("    moreleftspaces ") == "moreleftspaces");
    CHECK(RenderHair::trim("morerightspaces    ") == "morerightspaces");
}

TEST_CASE("Splitting checks")
{
    using strings = std::vector<std::string>;

    CHECK(RenderHair::split("", ' ') == strings{});
    CHECK(RenderHair::split("", 'x') == strings{});
    CHECK(RenderHair::split("with_delimiter", '_') == strings{"with", "delimiter"});
    CHECK(RenderHair::split("withoutYdelimiter", 'N') == strings{"withoutYdelimiter"});
    CHECK(RenderHair::split(" beginning with", ' ') == strings{"", "beginning", "with"});
    CHECK(RenderHair::split("ending_with_", '_') == strings{"ending", "with"});
    CHECK(RenderHair::split("=sequ==ential==", '=') == strings{"", "sequ", "", "ential", ""});
    CHECK(RenderHair::split("a lot of delimiters", ' ') == strings{"a", "lot", "of", "delimiters"});
}

TEST_CASE("Joining checks")
{
    CHECK(RenderHair::join({""}, "") == "");
    CHECK(RenderHair::join({"two str", "ings"}, "") == "two strings");
    CHECK(RenderHair::join({"two", "strings"}, " ") == "two strings");
    CHECK(RenderHair::join({"", "string", ""}, "") == "string");
    CHECK(RenderHair::join({"", "string", ""}, "__") == "__string__");
}
