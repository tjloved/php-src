--TEST--
Test to check regressions on string interpolation with class members access
--FILE--
<?php

class Friday
{
    public $require = "fun";
}
function fn663448801()
{
    $friday = new Friday();
    echo "{$friday->require} ({$friday->require}) {$friday->require}", PHP_EOL;
    echo "\nDone\n";
}
fn663448801();
--EXPECTF--

fun (fun) fun

Done
