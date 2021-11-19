#include "gurka.h"
#include <gtest/gtest.h>

#if !defined(VALHALLA_SOURCE_DIR)
#define VALHALLA_SOURCE_DIR
#endif

using namespace valhalla;

class RouteWithStreetnameAndSign_en_UnitedStates : public ::testing::Test {
protected:
  static gurka::map map;

  static void SetUpTestSuite() {
    constexpr double gridsize_metres = 100;

    const std::string ascii_map = R"(
                       J
                       |
                       |
                       |
                       I
                      /|\
                    /  |  \
                  /    |    \
           L----K-------------H----G
           A----B-------------E----F
                  \    |    /
                    \  |  /
                      \|/
                       C
                       |
                       |
                       |
                       D
               O------PM------Q
                       |
                       |
                       |
                       N

    )";

    const gurka::ways ways = {
        {"ABEF", {{"highway", "motorway"}, {"name", ""}, {"ref", "I 70"}, {"oneway", "yes"}}},
        {"GHKL", {{"highway", "motorway"}, {"name", ""}, {"ref", "I 70"}, {"oneway", "yes"}}},
        {"JICDMN",
         {{"highway", "primary"},
          {"name", "6th Avenue"},
          {"name:ru", "6-я авеню"},
          {"ref", "SR 37"}}},
        {"BC",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "126B"},
          {"destination", "York;Lancaster"},
          {"destination:lang:ru", "Йорк;Ланкастер"},
          {"destination:street", "6th Avenue"},
          {"destination:street:lang:ru", "6-я авеню"},
          {"destination:ref", "SR 37"}}},
        {"CE",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination:ref", "I 70 East"},
          {"destination:ref:lang:ru", "Я 70 Восток"}}},
        {"HI",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "126B"},
          {"destination:street:to", "Main Street"},
          {"destination:street:to:lang:ru", "Главная улица"},
          {"destination:ref:to", "I 80"},
          {"destination:ref:to:lang:ru", "Я 80"}}},
        {"IK",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination:ref", "I 70 West"},
          {"destination:ref:lang:ru", "Я 70 Запад"}}},
        {"OPMQ",
         {{"highway", "secondary"},
          {"name:en", "West 8th Street"},
          {"name:ru", "Западная 8-я стрит"}}},
        {"DP",
         {{"highway", "secondary_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination", "York"},
          {"destination:street", "West 8th Street"},
          {"destination:street:lang:ru", "Западная 8-я стрит"}}},
    };

    const gurka::nodes nodes = {{"M", {{"highway", "traffic_signals"}, {"name", "M Junction"}}}};

    const auto layout =
        gurka::detail::map_to_coordinates(ascii_map, gridsize_metres, {-82.68811, 40.22535});
    // TODO: determine the final name for language_admin.sqlite
    map = gurka::buildtiles(layout, ways, nodes, {},
                            "test/data/gurka_route_with_streetname_and_sign_en_UnitedStates",
                            {{"mjolnir.admin",
                              {VALHALLA_SOURCE_DIR "test/data/language_admin.sqlite"}}});
  }
};

gurka::map RouteWithStreetnameAndSign_en_UnitedStates::map = {};

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_en_UnitedStates, CheckStreetNamesAndSigns1) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"A", "D"}, "auto");
  gurka::assert::raw::expect_path(result, {"I 70", "", "6th Avenue/SR 37"});

  // Verify starting on I 70
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "I 70");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(0)
                .text(),
            "SR 37");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .text(),
            "6th Avenue");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "York");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Lancaster");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  // Verify street name language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "6th Avenue");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "SR 37");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(1)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_en_UnitedStates, CheckStreetNamesAndSigns2) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"G", "J"}, "auto");
  gurka::assert::raw::expect_path(result, {"I 70", "", "6th Avenue/SR 37"});

  // Verify starting on I 70
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "I 70");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "I 80");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Main Street");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_en_UnitedStates, CheckGuideSigns) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "O"}, "auto");
  gurka::assert::raw::expect_path(result, {"6th Avenue/SR 37", "6th Avenue/SR 37", "6th Avenue/SR 37",
                                           "", "West 8th Street"});

  // Verify starting on 6th Avenue/SR 37
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "6th Avenue");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "SR 37");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .text(),
            "West 8th Street");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .text(),
            "York");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_en_UnitedStates, CheckNonJunctionName) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "Q"}, "auto");
  gurka::assert::raw::expect_path(result, {"6th Avenue/SR 37", "6th Avenue/SR 37", "6th Avenue/SR 37",
                                           "6th Avenue/SR 37", "West 8th Street"});

  // Verify starting on 6th Avenue/SR 37
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "6th Avenue");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "SR 37");

  // Verify street name language tag is en
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "West 8th Street");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kEn);

  /*
  // Verify sign language tag is en
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .junction_names_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .junction_names(0)
                .text(),
            "M Junction");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .junction_names(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  // Verify junction name pronunciation instructions
  gurka::assert::raw::expect_instructions_at_maneuver_index(
      result, maneuver_index, "Turn left at M Junction.",
      "Turn left at M Junction (<span class=\"phoneme\">/ɛm ˈʤʌŋkʃən/</span>).",
      "Turn left at M Junction (<span class=\"phoneme\">/ɛm ˈʤʌŋkʃən/</span>).",
      "Turn left at M Junction (<span class=\"phoneme\">/ɛm ˈʤʌŋkʃən/</span>).",
      "Continue for 400 meters.");
*/
}

class RouteWithStreetnameAndSign_fr_nl_BrusselsBelgium : public ::testing::Test {
protected:
  static gurka::map map;

  static void SetUpTestSuite() {
    constexpr double gridsize_metres = 100;

    const std::string ascii_map = R"(
                       J
                       |
                       |
                       |
                       I
                      /|\
                    /  |  \
                  /    |    \
           L----K-------------H----G
           A----B-------------E----F
                  \    |    /
                    \  |  /
                      \|/
                       C
                       |
                       |
                       |
                       D
               O------PM------Q
                       |
                       |
                       |
                       N

    )";

    const gurka::ways ways = {
        {"ABEF", {{"highway", "motorway"}, {"name", ""}, {"ref", "E40"}, {"oneway", "yes"}}},
        {"GHKL", {{"highway", "motorway"}, {"name", ""}, {"ref", "E40"}, {"oneway", "yes"}}},
        {"JICDMN",
         {{"highway", "primary"},
          {"name", "Rue Bodenbroek - Bodenbroekstraat"},
          {"name:fr", "Rue Bodenbroek"},
          {"name:nl", "Bodenbroekstraat"},
          {"ref", "N6"}}},
        {"BC",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "12"},
          {"destination", "Brussel;Namen"},
          {"destination:street", "Bodenbroekstraat"},
          {"destination:street:lang:fr", "Rue Bodenbroek"},
          {"destination:street:lang:nl", "Bodenbroekstraat"},
          {"destination:ref", "N6"}}},
        {"CE",
         {{"highway", "motorway_link"}, {"name", ""}, {"oneway", "yes"}, {"destination:ref", "E40"}}},
        {"HI",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "12"},
          {"destination:street:to", "Koningsstraat"},
          {"destination:street:to:lang:fr", "Rue Royale"},
          {"destination:street:to:lang:nl", "Koningsstraat"},
          {"destination:ref:to", "E19"}}},
        {"IK",
         {{"highway", "motorway_link"}, {"name", ""}, {"oneway", "yes"}, {"destination:ref", "E40"}}},
        {"OPMQ",
         {{"highway", "secondary"},
          {"name", "Rue Lebeau - Lebeaustraat"},
          {"name:fr", "Rue Lebeau"},
          {"name:nl", "Lebeaustraat"}}},
        {"DP",
         {{"highway", "secondary_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination", "Brussel"},
          {"destination:street", "Lebeaustraat"},
          {"destination:street:lang:fr", "Rue Lebeau"},
          {"destination:street:lang:nl", "Lebeaustraat"}}},
    };

    const gurka::nodes nodes = {{"M", {{"highway", "traffic_signals"}, {"name", "Zaventem"}}}};

    const auto layout =
        gurka::detail::map_to_coordinates(ascii_map, gridsize_metres, {4.3516970, 50.8465573});
    // TODO: determine the final name for language_admin.sqlite
    map = gurka::buildtiles(layout, ways, nodes, {},
                            "test/data/gurka_route_with_streetname_and_sign_fr_nl_BrusselsBelgium",
                            {{"mjolnir.admin",
                              {VALHALLA_SOURCE_DIR "test/data/language_admin.sqlite"}}});
  }
};

gurka::map RouteWithStreetnameAndSign_fr_nl_BrusselsBelgium::map = {};

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_fr_nl_BrusselsBelgium, CheckStreetNamesAndSigns1) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"A", "D"}, "auto");
  gurka::assert::raw::expect_path(result, {"E40", "", "Rue Bodenbroek/Bodenbroekstraat/N6"});

  // Verify starting on E40
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "E40");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(0)
                .text(),
            "N6");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .text(),
            "Bodenbroekstraat");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "Brussel");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Namen");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  // Verify street name language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 3);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "Rue Bodenbroek");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kFr);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "Bodenbroekstraat");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(1)
                .language_tag(),
            LanguageTag::kNl);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(2).value(),
            "N6");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(2)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_fr_nl_BrusselsBelgium, CheckStreetNamesAndSigns2) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"G", "J"}, "auto");
  gurka::assert::raw::expect_path(result, {"E40", "", "Rue Bodenbroek/Bodenbroekstraat/N6"});

  // Verify starting on E40
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "E40");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "E19");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Koningsstraat");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_fr_nl_BrusselsBelgium, CheckGuideSigns) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "O"}, "auto");
  gurka::assert::raw::expect_path(result, {"Rue Bodenbroek/Bodenbroekstraat/N6",
                                           "Rue Bodenbroek/Bodenbroekstraat/N6",
                                           "Rue Bodenbroek/Bodenbroekstraat/N6", "",
                                           "Rue Lebeau/Lebeaustraat"});

  // Verify starting on Rue Bodenbroek/Bodenbroekstraat/N6
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 3);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "Rue Bodenbroek");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "Bodenbroekstraat");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(2).value(),
            "N6");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .text(),
            "Lebeaustraat");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .text(),
            "Brussel");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_fr_nl_BrusselsBelgium, CheckNonJunctionName) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "Q"}, "auto");
  gurka::assert::raw::expect_path(result,
                                  {"Rue Bodenbroek/Bodenbroekstraat/N6",
                                   "Rue Bodenbroek/Bodenbroekstraat/N6",
                                   "Rue Bodenbroek/Bodenbroekstraat/N6",
                                   "Rue Bodenbroek/Bodenbroekstraat/N6", "Rue Lebeau/Lebeaustraat"});

  // Verify starting on Rue Bodenbroek/Bodenbroekstraat/N6
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 3);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "Rue Bodenbroek");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "Bodenbroekstraat");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(2).value(),
            "N6");

  // Verify street name language tag is en
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "Rue Lebeau");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kFr);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "Lebeaustraat");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(1)
                .language_tag(),
            LanguageTag::kNl);
}

class RouteWithStreetnameAndSign_ru_be_MinskBelarus : public ::testing::Test {
protected:
  static gurka::map map;

  static void SetUpTestSuite() {
    constexpr double gridsize_metres = 100;

    const std::string ascii_map = R"(
                       J
                       |
                       |
                       |
                       I
                      /|\
                    /  |  \
                  /    |    \
           L----K-------------H----G
           A----B-------------E----F
                  \    |    /
                    \  |  /
                      \|/
                       C
                       |
                       |
                       |
                       D
               O------PM------Q
                       |
                       |
                       |
                       N

    )";

    const gurka::ways ways = {
        {"ABEF", {{"highway", "motorway"}, {"name", ""}, {"ref", "М2"}, {"oneway", "yes"}}},
        {"GHKL", {{"highway", "motorway"}, {"name", ""}, {"ref", "М2"}, {"oneway", "yes"}}},
        {"JICDMN",
         {{"highway", "primary"},
          {"name", "МКАД, 1-й километр"},
          {"name:ru", "МКАД, 1-й километр"},
          {"name:be", "1-ы кіламетр МКАД"},
          {"ref", "M9"}}},
        {"BC",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "12"},
          {"destination", "Гомель;Слуцк"},
          {"destination:street", "1-ы кіламетр МКАД"},
          {"destination:street:lang:ru", "МКАД, 1-й километр"},
          {"destination:street:lang:be", "1-ы кіламетр МКАД"},
          {"destination:ref", "M9"}}},
        {"CE",
         {{"highway", "motorway_link"}, {"name", ""}, {"oneway", "yes"}, {"destination:ref", "М2"}}},
        {"HI",
         {{"highway", "motorway_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"junction:ref", "12"},
          {"destination:street:to", "Партизанский проспект"},
          {"destination:street:to:lang:ru", "Партизанский проспект"},
          {"destination:street:to:lang:be", "Партызанскі праспект"},
          {"destination:ref:to", "M4"}}},
        {"IK",
         {{"highway", "motorway_link"}, {"name", ""}, {"oneway", "yes"}, {"destination:ref", "М2"}}},
        {"OPMQ",
         {{"highway", "secondary"},
          {"name", "Днепровская улица"},
          {"name:ru", "Днепровская улица"},
          {"name:be", "Дняпроўская вуліца"}}},
        {"DP",
         {{"highway", "secondary_link"},
          {"name", ""},
          {"oneway", "yes"},
          {"destination", "Гомель"},
          {"destination:street", "Днепровская улица"},
          {"destination:street:lang:ru", "Днепровская улица"},
          {"destination:street:lang:be", "Дняпроўская вуліца"}}},
    };

    const gurka::nodes nodes = {{"M", {{"highway", "traffic_signals"}, {"name", "Zaventem"}}}};

    const auto layout =
        gurka::detail::map_to_coordinates(ascii_map, gridsize_metres, {27.56191, 53.90246});
    // TODO: determine the final name for language_admin.sqlite
    map = gurka::buildtiles(layout, ways, nodes, {},
                            "test/data/gurka_route_with_streetname_and_sign_ru_be_MinskBelarus",
                            {{"mjolnir.admin",
                              {VALHALLA_SOURCE_DIR "test/data/language_admin.sqlite"}}});
  }
};

gurka::map RouteWithStreetnameAndSign_ru_be_MinskBelarus::map = {};

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_ru_be_MinskBelarus, CheckStreetNamesAndSigns1) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"A", "D"}, "auto");
  gurka::assert::raw::expect_path(result, {"М2", "", "МКАД, 1-й километр/1-ы кіламетр МКАД/M9"});

  // Verify starting on М2
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "М2");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(0)
                .text(),
            "M9");

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .text(),
            "1-ы кіламетр МКАД");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_onto_streets(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "Гомель");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Слуцк");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);

  // Verify street name language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 3);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "МКАД, 1-й километр");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kRu);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "1-ы кіламетр МКАД");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(1)
                .language_tag(),
            LanguageTag::kBe);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(2).value(),
            "M9");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(2)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_ru_be_MinskBelarus, CheckStreetNamesAndSigns2) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"G", "J"}, "auto");
  gurka::assert::raw::expect_path(result, {"М2", "", "МКАД, 1-й километр/1-ы кіламетр МКАД/M9"});

  // Verify starting on М2
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 1);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "М2");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations_size(),
            2);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .text(),
            "M4");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .text(),
            "Партизанский проспект");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .exit_toward_locations(1)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_ru_be_MinskBelarus, CheckGuideSigns) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "O"}, "auto");
  gurka::assert::raw::expect_path(result, {"МКАД, 1-й километр/1-ы кіламетр МКАД/M9",
                                           "МКАД, 1-й километр/1-ы кіламетр МКАД/M9",
                                           "МКАД, 1-й километр/1-ы кіламетр МКАД/M9", "",
                                           "Днепровская улица/Дняпроўская вуліца"});

  // Verify starting on МКАД, 1-й километр/1-ы кіламетр МКАД/M9
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 3);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "МКАД, 1-й километр");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "1-ы кіламетр МКАД");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(2).value(),
            "M9");

  // Verify sign language tag is en
  // TODO: after logic is updated then change LanguageTag::kUnspecified to LanguageTag::kEn
  ++maneuver_index;
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .text(),
            "Днепровская улица");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_onto_streets(0)
                .language_tag(),
            LanguageTag::kUnspecified);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations_size(),
            1);

  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .text(),
            "Гомель");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .sign()
                .guide_toward_locations(0)
                .language_tag(),
            LanguageTag::kUnspecified);
}

///////////////////////////////////////////////////////////////////////////////
TEST_F(RouteWithStreetnameAndSign_ru_be_MinskBelarus, CheckNonJunctionName) {
  auto result = gurka::do_action(valhalla::Options::route, map, {"J", "Q"}, "auto");
  gurka::assert::raw::expect_path(result, {"МКАД, 1-й километр/1-ы кіламетр МКАД/M9",
                                           "МКАД, 1-й километр/1-ы кіламетр МКАД/M9",
                                           "МКАД, 1-й километр/1-ы кіламетр МКАД/M9",
                                           "МКАД, 1-й километр/1-ы кіламетр МКАД/M9",
                                           "Днепровская улица/Дняпроўская вуліца"});

  // Verify starting on МКАД, 1-й километр/1-ы кіламетр МКАД/M9
  int maneuver_index = 0;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 3);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "МКАД, 1-й километр");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "1-ы кіламетр МКАД");
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(2).value(),
            "M9");

  // Verify street name language tag is en
  ++maneuver_index;
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name_size(), 2);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(0).value(),
            "Днепровская улица");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(0)
                .language_tag(),
            LanguageTag::kRu);
  EXPECT_EQ(result.directions().routes(0).legs(0).maneuver(maneuver_index).street_name(1).value(),
            "Дняпроўская вуліца");
  EXPECT_EQ(result.directions()
                .routes(0)
                .legs(0)
                .maneuver(maneuver_index)
                .street_name(1)
                .language_tag(),
            LanguageTag::kBe);
}
