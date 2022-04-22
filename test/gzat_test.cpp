/*
 * MIT License
 * 
 * Copyright (c) 2020 Guanyu Zhou
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "gzat_parser.hpp"
#include <gtest/gtest.h> // IWYU pragma: keep 
// IWYU pragma: no_include <gtest/gtest-message.h>
// IWYU pragma: no_include <gtest/gtest-test-part.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

using gzat::AtCommand;
using gzat::CommandParser;
using gzat::CommaSplitParser;

TEST(GZATParse, ConstructNoMS)
{
    AtCommand atcmd("ATZ");
    EXPECT_EQ(atcmd.ms, 0);
    EXPECT_EQ(atcmd.me, 0);
    EXPECT_STREQ(atcmd.cmd_id.c_str(), "Z");
    EXPECT_TRUE(atcmd.cmd_payload.empty());
    EXPECT_STREQ(atcmd.GetRawCommand().c_str(), "ATZ");
}

TEST(GZATParse, ConstructMS)
{
    AtCommand atcmd("AT+Z");
    EXPECT_EQ(atcmd.ms, 1);
    EXPECT_EQ(atcmd.me, 0);
    EXPECT_STREQ(atcmd.cmd_id.c_str(), "+Z");
    EXPECT_TRUE(atcmd.cmd_payload.empty());
    EXPECT_STREQ(atcmd.GetRawCommand().c_str(), "AT+Z");
}

TEST(GZATParse, ConstructMSTest)
{
    AtCommand atcmd("AT#Z=?");
    EXPECT_EQ(atcmd.ms, 2);
    EXPECT_EQ(atcmd.me, 1);
    EXPECT_STREQ(atcmd.cmd_id.c_str(), "#Z");
    EXPECT_TRUE(atcmd.cmd_payload.empty());
    EXPECT_STREQ(atcmd.GetRawCommand().c_str(), "AT#Z=?");
}

TEST(GZATParse, ConstructMSGet)
{
    AtCommand atcmd("AT+CSQ?");
    EXPECT_EQ(atcmd.ms, 1);
    EXPECT_EQ(atcmd.me, 2);
    EXPECT_STREQ(atcmd.cmd_id.c_str(), "+CSQ");
    EXPECT_TRUE(atcmd.cmd_payload.empty());
    EXPECT_STREQ(atcmd.GetRawCommand().c_str(), "AT+CSQ?");
}

TEST(GZATParse, ConstructMSSet)
{
    AtCommand atcmd("AT+ABC=1,\"abc\"");
    EXPECT_EQ(atcmd.ms, 1);
    EXPECT_EQ(atcmd.me, 3);
    EXPECT_STREQ(atcmd.cmd_id.c_str(), "+ABC");
    EXPECT_STREQ(atcmd.cmd_payload.c_str(), "1,\"abc\"");
    EXPECT_STREQ(atcmd.GetRawCommand().c_str(), "AT+ABC=1,\"abc\"");
}

TEST(GZATParse, ParseMSGetInt)
{
    AtCommand atcmd("AT+CSQ?");
    CommandParser parser(atcmd);
    std::shared_ptr<CommaSplitParser> csp1 = std::make_shared<CommaSplitParser>(0);
    int64_t p1_int = 0;
    csp1->AddIntegerOutput(&p1_int);
    std::shared_ptr<CommaSplitParser> csp2 = std::make_shared<CommaSplitParser>(1);
    int64_t p2_int = 0;
    csp2->AddIntegerOutput(&p2_int);
    parser.AddChildParser(csp1).AddChildParser(csp2);
    parser.Parse("+CSQ: 10,100");
    EXPECT_EQ(p1_int, 10);
    EXPECT_EQ(p2_int, 100);
}

TEST(GZATParse, ParseMSGetString)
{
    AtCommand atcmd("AT+PDP?");
    CommandParser parser(atcmd);
    std::shared_ptr<CommaSplitParser> csp1 = std::make_shared<CommaSplitParser>(0);
    int64_t p1_int = 0;
    csp1->AddIntegerOutput(&p1_int);
    std::shared_ptr<CommaSplitParser> csp2 = std::make_shared<CommaSplitParser>(1);
    std::string p2_string;
    csp2->AddStringOutput(&p2_string);
    std::shared_ptr<CommaSplitParser> csp3 = std::make_shared<CommaSplitParser>(2);
    std::string p3_string;
    csp3->AddStringOutput(&p3_string);
    parser.AddChildParser(csp1).AddChildParser(csp2).AddChildParser(csp3);
    parser.Parse("+PDP: 10,\"1.2.3.4\",abc\r\rOK");
    EXPECT_EQ(p1_int, 10);
    EXPECT_STREQ(p2_string.c_str(), "1.2.3.4");
    EXPECT_STREQ(p3_string.c_str(), "abc");
}

TEST(GZATParse, ParseMSGetIntDirty)
{
    AtCommand atcmd("AT+CSQ?");
    CommandParser parser(atcmd);
    std::shared_ptr<CommaSplitParser> csp1 = std::make_shared<CommaSplitParser>(0);
    int64_t p1_int = 0;
    csp1->AddIntegerOutput(&p1_int);
    std::shared_ptr<CommaSplitParser> csp2 = std::make_shared<CommaSplitParser>(1);
    int64_t p2_int = 0;
    csp2->AddIntegerOutput(&p2_int);
    parser.AddChildParser(csp1).AddChildParser(csp2);
    parser.Parse("AT+CSQ?\r\r+CSQ: 10,100\r\rOK");
    EXPECT_EQ(p1_int, 10);
    EXPECT_EQ(p2_int, 100);
}

TEST(GZATParse, ParseMSGetStringDirty)
{
    AtCommand atcmd("AT+PDP?");
    CommandParser parser(atcmd);
    std::shared_ptr<CommaSplitParser> csp1 = std::make_shared<CommaSplitParser>(0);
    int64_t p1_int = 0;
    csp1->AddIntegerOutput(&p1_int);
    std::shared_ptr<CommaSplitParser> csp2 = std::make_shared<CommaSplitParser>(1);
    std::string p2_string;
    csp2->AddStringOutput(&p2_string);
    std::shared_ptr<CommaSplitParser> csp3 = std::make_shared<CommaSplitParser>(2);
    std::string p3_string;
    csp3->AddStringOutput(&p3_string);
    parser.AddChildParser(csp1).AddChildParser(csp2).AddChildParser(csp3);
    parser.Parse("AT+PDP?\r\r+PDP: 10,\"1.2.3.4\",\"abc\"    \r\r   OK");
    EXPECT_EQ(p1_int, 10);
    EXPECT_STREQ(p2_string.c_str(), "1.2.3.4");
    EXPECT_STREQ(p3_string.c_str(), "abc");
}