--TEST--
Testing nested array shortcut
--FILE--
<?php

function fn176324641()
{
    print_r([1, 2, 3, ["foo" => "orange", "bar" => "apple", "baz" => "lemon"]]);
}
fn176324641();
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => Array
        (
            [foo] => orange
            [bar] => apple
            [baz] => lemon
        )

)
