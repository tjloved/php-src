--TEST--
Bug #23624 (foreach leaves current array key as null)
--FILE--
<?php

function fn3323852()
{
    $arr = array('one', 'two', 'three');
    var_dump(current($arr));
    foreach ($arr as $key => $value) {
    }
    var_dump(current($arr));
}
fn3323852();
--EXPECT--
string(3) "one"
string(3) "one"
