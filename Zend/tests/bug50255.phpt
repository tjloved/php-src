--TEST--
Bug #50255 (isset() and empty() silently casts array to object)
--FILE--
<?php

function fn1951368881()
{
    $arr = array('foo' => 'bar');
    print "isset\n";
    var_dump(isset($arr->foo));
    var_dump(isset($arr->bar));
    var_dump(isset($arr['foo']));
    var_dump(isset($arr['bar']));
    print "empty\n";
    var_dump(empty($arr->foo));
    var_dump(empty($arr->bar));
    var_dump(empty($arr['foo']));
    var_dump(empty($arr['bar']));
}
fn1951368881();
--EXPECT--
isset
bool(false)
bool(false)
bool(true)
bool(false)
empty
bool(true)
bool(true)
bool(false)
bool(true)
