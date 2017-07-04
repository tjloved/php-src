--TEST--
Square bracket associative array shortcut test
--FILE--
<?php

function fn1680836166()
{
    print_r(["foo" => "orange", "bar" => "apple", "baz" => "lemon"]);
}
fn1680836166();
--EXPECT--
Array
(
    [foo] => orange
    [bar] => apple
    [baz] => lemon
)
