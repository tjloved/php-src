--TEST--
Bug #34786 (2 @ results in change to error_reporting() to random value)
--FILE--
<?php

function foo($a, $b, $c)
{
    echo "foo: " . error_reporting() . "\n";
}
function bar()
{
    echo "bar: " . error_reporting() . "\n";
}
function fn711037450()
{
    error_reporting(1);
    echo "before: " . error_reporting() . "\n";
    @foo(1, @bar(), 3);
    echo "after: " . error_reporting() . "\n";
}
fn711037450();
--EXPECT--
before: 1
bar: 0
foo: 0
after: 1
