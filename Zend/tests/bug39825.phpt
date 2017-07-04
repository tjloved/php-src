--TEST--
Bug #39825 (foreach produces memory error)
--FILE--
<?php

function fn1092491892()
{
    $array = array(1 => 2, "foo" => "bar");
    $obj = (object) $array;
    foreach ($obj as $name => $value) {
        echo "{$name} -> {$value}\n";
    }
}
fn1092491892();
--EXPECT--
1 -> 2
foo -> bar
