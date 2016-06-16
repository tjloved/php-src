--TEST--
Test to check regressions on string interpolation with class members access
--FILE--
<?php

class Friday
{
    public $require = "fun";
}
function fn606693762()
{
    $friday = new Friday();
    echo "{$friday->require} ({$friday->require}) {$friday->require}", PHP_EOL;
    echo "\nDone\n";
}
fn606693762();
--EXPECTF--

fun (fun) fun

Done
