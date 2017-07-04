--TEST--
Bug #71914 (Reference is lost in "switch")
--FILE--
<?php

function bug(&$value)
{
    switch ($value) {
        case "xxxx":
            $value = true;
            break;
    }
}
function returnArray()
{
    $array = array();
    $array["str"] = "xxxx";
    return $array;
}
class Foo
{
    public $array = array("str" => "xxxx");
}
function test($arr, &$dummy)
{
    bug($arr["str"]);
    var_dump($arr["str"]);
}
function fn997147090()
{
    $foo = new Foo();
    $arr = returnArray();
    $array = array("str" => "xxxx");
    test($array, $array["str"]);
    test($arr, $arr["str"]);
    test($foo->array, $foo->array["str"]);
}
fn997147090();
--EXPECT--
bool(true)
bool(true)
bool(true)
