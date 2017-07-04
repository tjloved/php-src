--TEST--
Tolerated Unicode escape sequences: Legacy compatibility
--FILE--
<?php

function fn352646946()
{
    // These are ignored to avoid breaking JSON string literals
    var_dump("\\u");
    var_dump("\\u202e");
    var_dump("\\ufoobar");
}
fn352646946();
--EXPECTF--
string(2) "\u"
string(6) "\u202e"
string(8) "\ufoobar"
