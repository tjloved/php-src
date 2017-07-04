--TEST--
Bug #69889: Null coalesce operator doesn't work for string offsets
--FILE--
<?php

function fn1745488113()
{
    $foo = "test";
    var_dump($foo[0] ?? "default");
    var_dump($foo[5] ?? "default");
    var_dump(isset($foo[5]) ? $foo[5] : "default");
    var_dump($foo["str"] ?? "default");
    var_dump(isset($foo["str"]) ? $foo["str"] : "default");
}
fn1745488113();
--EXPECT--
string(1) "t"
string(7) "default"
string(7) "default"
string(7) "default"
string(7) "default"

