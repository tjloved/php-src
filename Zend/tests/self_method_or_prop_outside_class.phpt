--TEST--
Accessing self:: properties or methods outside a class
--FILE--
<?php

function fn1542739780()
{
    $fn = function () {
        $str = "foo";
        try {
            self::${$str . "bar"};
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            unset(self::${$str . "bar"});
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            isset(self::${$str . "bar"});
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            self::{$str . "bar"}();
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    };
    $fn();
}
fn1542739780();
--EXPECT--
Cannot access self:: when no class scope is active
Cannot access self:: when no class scope is active
Cannot access self:: when no class scope is active
Cannot access self:: when no class scope is active
