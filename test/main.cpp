#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru_RU");

    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}