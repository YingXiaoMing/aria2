#include "AnnounceList.h"
#include "MetaFileUtil.h"
#include "Exception.h"
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

class AnnounceListTest:public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(AnnounceListTest);
  CPPUNIT_TEST(testSingleElementList);
  CPPUNIT_TEST(testMultiElementList);
  CPPUNIT_TEST(testSingleAndMulti);
  CPPUNIT_TEST(testNoGroup);
  CPPUNIT_TEST(testCountStoppedAllowedTier);
  CPPUNIT_TEST(testCountCompletedAllowedTier);
  CPPUNIT_TEST(testMoveToStoppedAllowedTier);
  CPPUNIT_TEST(testMoveToCompletedAllowedTier);
  CPPUNIT_TEST_SUITE_END();
private:

public:
  void setUp() {
  }

  void testSingleElementList();
  void testMultiElementList();
  void testSingleAndMulti();
  void testNoGroup();
  void testEvent();
  void testCountStoppedAllowedTier();
  void testCountCompletedAllowedTier();
  void testMoveToStoppedAllowedTier();
  void testMoveToCompletedAllowedTier();
};


CPPUNIT_TEST_SUITE_REGISTRATION( AnnounceListTest );

void AnnounceListTest::testSingleElementList() {
  string peersString = "ll8:tracker1el8:tracker2el8:tracker3ee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());

  // ANNOUNCE_LIST
  // [ [ tracker1 ], [ tracker2 ], [ tracker3 ] ]
  AnnounceList announceList(announces);
  
  CPPUNIT_ASSERT(!announceList.allTiersFailed());
  string url =  announceList.getAnnounce();
  string event = announceList.getEventString();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  CPPUNIT_ASSERT_EQUAL(string("started"), event);
  announceList.announceFailure();
  url =  announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
  announceList.announceFailure();
  url =  announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker3"), url);
  announceList.announceFailure();
  CPPUNIT_ASSERT(announceList.allTiersFailed());
  announceList.resetTier();
  CPPUNIT_ASSERT(!announceList.allTiersFailed());
  // back to the first list
  url = announceList.getAnnounce();
  event = announceList.getEventString();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  CPPUNIT_ASSERT_EQUAL(string("started"), event);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  event = announceList.getEventString();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
  CPPUNIT_ASSERT_EQUAL(string("started"), event);
  announceList.announceSuccess();
  // back to the first list because announce to tracker2 succeeded.
  url = announceList.getAnnounce();
  event = announceList.getEventString();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  CPPUNIT_ASSERT_EQUAL(string("started"), event);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  event = announceList.getEventString();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
  CPPUNIT_ASSERT_EQUAL(string(""), event);
}

void AnnounceListTest::testMultiElementList() {
  string peersString = "ll8:tracker18:tracker28:tracker3ee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());
  // ANNOUNCE_LIST
  // [ [ tracker1, tracker2, tracker3 ] ]
  AnnounceList announceList(announces);
  
  CPPUNIT_ASSERT(!announceList.allTiersFailed());
  string url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
  announceList.announceSuccess();
  url = announceList.getAnnounce();
  // tracker2 returns because tracker2 is now first.
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker3"), url);
  announceList.announceFailure();
  CPPUNIT_ASSERT(announceList.allTiersFailed());
  announceList.resetTier();
  CPPUNIT_ASSERT(!announceList.allTiersFailed());
  // back to the first list because there is no other list.
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
}

void AnnounceListTest::testSingleAndMulti() {
  string peersString = "ll8:tracker18:tracker2el8:tracker3ee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());

  // ANNOUNCE_LIST
  // [ [ tracker1, tracker2 ], [ tracker3 ] ]
  AnnounceList announceList(announces);

  string url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  announceList.announceSuccess();
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), url);
  announceList.announceFailure();
  url = announceList.getAnnounce();
  CPPUNIT_ASSERT_EQUAL(string("tracker3"), url);
  announceList.announceSuccess();
  url = announceList.getAnnounce();
  // tracker1 returns because after the announce to tracker3 succeeds, list
  // pointer points to the first list.
  CPPUNIT_ASSERT_EQUAL(string("tracker1"), url);
}

void AnnounceListTest::testNoGroup() {
  string peersString = "llee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());

  AnnounceList announceList(announces);

  CPPUNIT_ASSERT(announceList.countTier() == 0);
}

void AnnounceListTest::testEvent() {
  string peersString = "ll8:tracker1el8:tracker2el8:tracker3ee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());

  // ANNOUNCE_LIST
  // [ [ tracker1 ], [ tracker2 ], [ tracker3 ] ]
  AnnounceList announceList(announces);

  announceList.setEvent(AnnounceTier::STOPPED);
  announceList.announceSuccess();
  CPPUNIT_ASSERT_EQUAL(string(""), announceList.getEventString());
  CPPUNIT_ASSERT_EQUAL(AnnounceTier::HALTED, announceList.getEvent());

  announceList.setEvent(AnnounceTier::COMPLETED);
  announceList.announceSuccess();
  CPPUNIT_ASSERT_EQUAL(string(""), announceList.getEventString());
  CPPUNIT_ASSERT_EQUAL(AnnounceTier::SEEDING, announceList.getEvent());

  announceList.setEvent(AnnounceTier::STARTED_AFTER_COMPLETION);
  CPPUNIT_ASSERT_EQUAL(string("started"), announceList.getEventString());
  announceList.announceSuccess();
  CPPUNIT_ASSERT_EQUAL(AnnounceTier::SEEDING, announceList.getEvent());
}

void AnnounceListTest::testCountStoppedAllowedTier() {
  string peersString = "ll8:tracker1el8:tracker2el8:tracker3ee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());

  // ANNOUNCE_LIST
  // [ [ tracker1 ], [ tracker2 ], [ tracker3 ] ]
  AnnounceList announceList(announces);

  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countStoppedAllowedTier());
  announceList.setEvent(AnnounceTier::STARTED);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countStoppedAllowedTier());
  announceList.setEvent(AnnounceTier::STARTED_AFTER_COMPLETION);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countStoppedAllowedTier());
  announceList.setEvent(AnnounceTier::HALTED);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countStoppedAllowedTier());

  announceList.setEvent(AnnounceTier::DOWNLOADING);
  CPPUNIT_ASSERT_EQUAL((int32_t)1, announceList.countStoppedAllowedTier());
  announceList.setEvent(AnnounceTier::STOPPED);
  CPPUNIT_ASSERT_EQUAL((int32_t)1, announceList.countStoppedAllowedTier());
  announceList.setEvent(AnnounceTier::COMPLETED);
  CPPUNIT_ASSERT_EQUAL((int32_t)1, announceList.countStoppedAllowedTier());
  announceList.setEvent(AnnounceTier::SEEDING);
  CPPUNIT_ASSERT_EQUAL((int32_t)1, announceList.countStoppedAllowedTier());

}

void AnnounceListTest::testCountCompletedAllowedTier() {
  string peersString = "ll8:tracker1el8:tracker2el8:tracker3ee";
  Dictionary* announces = (Dictionary*)MetaFileUtil::bdecoding(peersString.c_str(), peersString.size());

  // ANNOUNCE_LIST
  // [ [ tracker1 ], [ tracker2 ], [ tracker3 ] ]
  AnnounceList announceList(announces);

  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countCompletedAllowedTier());
  announceList.setEvent(AnnounceTier::STARTED);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countCompletedAllowedTier());
  announceList.setEvent(AnnounceTier::STARTED_AFTER_COMPLETION);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countCompletedAllowedTier());
  announceList.setEvent(AnnounceTier::STOPPED);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countCompletedAllowedTier());
  announceList.setEvent(AnnounceTier::SEEDING);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countCompletedAllowedTier());
  announceList.setEvent(AnnounceTier::HALTED);
  CPPUNIT_ASSERT_EQUAL((int32_t)0, announceList.countCompletedAllowedTier());

  announceList.setEvent(AnnounceTier::DOWNLOADING);
  CPPUNIT_ASSERT_EQUAL((int32_t)1, announceList.countCompletedAllowedTier());
  announceList.setEvent(AnnounceTier::COMPLETED);
  CPPUNIT_ASSERT_EQUAL((int32_t)1, announceList.countCompletedAllowedTier());

}

Strings createUrls(const string& url) {
  Strings urls;
  urls.push_back(url);
  return urls;
}

void AnnounceListTest::testMoveToStoppedAllowedTier() {
  AnnounceTierHandle t1(new AnnounceTier(createUrls("tracker1")));
  AnnounceTierHandle t2(new AnnounceTier(createUrls("tracker2")));
  t2->event = AnnounceTier::COMPLETED;
  AnnounceTierHandle t3(new AnnounceTier(createUrls("tracker3")));

  AnnounceTiers tiers;
  tiers.push_back(t1);
  tiers.push_back(t2);
  tiers.push_back(t3);

  AnnounceList announceList(tiers);

  CPPUNIT_ASSERT_EQUAL(string("tracker1"), announceList.getAnnounce());
  announceList.moveToStoppedAllowedTier();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), announceList.getAnnounce());
  announceList.announceFailure();
  CPPUNIT_ASSERT_EQUAL(string("tracker3"), announceList.getAnnounce());
  // test wrapped search
  announceList.moveToStoppedAllowedTier();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), announceList.getAnnounce());  
}

void AnnounceListTest::testMoveToCompletedAllowedTier() {
  AnnounceTierHandle t1(new AnnounceTier(createUrls("tracker1")));
  AnnounceTierHandle t2(new AnnounceTier(createUrls("tracker2")));
  t2->event = AnnounceTier::COMPLETED;
  AnnounceTierHandle t3(new AnnounceTier(createUrls("tracker3")));

  AnnounceTiers tiers;
  tiers.push_back(t1);
  tiers.push_back(t2);
  tiers.push_back(t3);

  AnnounceList announceList(tiers);

  CPPUNIT_ASSERT_EQUAL(string("tracker1"), announceList.getAnnounce());
  announceList.moveToStoppedAllowedTier();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), announceList.getAnnounce());
  announceList.announceFailure();
  CPPUNIT_ASSERT_EQUAL(string("tracker3"), announceList.getAnnounce());
  // test wrapped search
  announceList.moveToStoppedAllowedTier();
  CPPUNIT_ASSERT_EQUAL(string("tracker2"), announceList.getAnnounce());  
}
