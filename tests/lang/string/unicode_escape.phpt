--TEST--
Valid Unicode escape sequences
--FILE--
<?php

function fn1615964551()
{
    var_dump("a");
    // ASCII "a" - characters below U+007F just encode as ASCII, as it's UTF-8
    var_dump("ÿ");
    // y with diaeresis
    var_dump("ÿ");
    // case-insensitive
    var_dump("☃");
    // Unicode snowman
    var_dump("😂");
    // FACE WITH TEARS OF JOY emoji
    var_dump("😂");
    // Leading zeroes permitted
}
fn1615964551();
--EXPECT--
string(1) "a"
string(2) "ÿ"
string(2) "ÿ"
string(3) "☃"
string(4) "😂"
string(4) "😂"
