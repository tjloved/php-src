--TEST--
Square bracket associative array shortcut test
--FILE--
<?php

function fn1749909425()
{
    print_r(["foo" => "orange", "bar" => "apple", "baz" => "lemon"]);
}
fn1749909425();
--EXPECT--
Array
(
    [foo] => orange
    [bar] => apple
    [baz] => lemon
)
