--TEST--
Bug #21600 (assign by reference function call changes variable contents)
--INI--
error_reporting=4095
--FILE--
<?php

function bar($text)
{
    return $text;
}
function fubar($text)
{
    $text =& $text;
    return $text;
}
function fn524848429()
{
    $tmp = array();
    $tmp['foo'] = "test";
    $tmp['foo'] =& bar($tmp['foo']);
    var_dump($tmp);
    unset($tmp);
    $tmp = array();
    $tmp['foo'] = "test";
    $tmp['foo'] =& fubar($tmp['foo']);
    var_dump($tmp);
}
fn524848429();
--EXPECTF--
Notice: Only variables should be assigned by reference in %sbug21600.php on line %d
array(1) {
  ["foo"]=>
  string(4) "test"
}

Notice: Only variables should be assigned by reference in %sbug21600.php on line %d
array(1) {
  ["foo"]=>
  string(4) "test"
}
