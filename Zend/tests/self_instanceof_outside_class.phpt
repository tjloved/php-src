--TEST--
instanceof self outside a class
--FILE--
<?php

function fn79512385()
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
fn79512385();
--EXPECT--
Cannot access self:: when no class scope is active
