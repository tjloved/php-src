--TEST--
Bug #39825 (foreach produces memory error)
--FILE--
<?php

function fn1993453367()
{
    $array = array(1 => 2, "foo" => "bar");
    $obj = (object) $array;
    foreach ($obj as $name => $value) {
        echo "{$name} -> {$value}\n";
    }
}
fn1993453367();
--EXPECT--
1 -> 2
foo -> bar
