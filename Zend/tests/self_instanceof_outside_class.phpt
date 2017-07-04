--TEST--
instanceof self outside a class
--FILE--
<?php

function fn229890887()
{
    $fn = function () {
        try {
            new stdClass() instanceof self;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    };
    $fn();
}
fn229890887();
--EXPECT--
Cannot access self:: when no class scope is active
