--TEST--
Ensure catch blocks for unknown exception types do not trigger autoload.
--FILE--
<?php

function f()
{
    throw new Exception();
}
function fn1366733998()
{
    spl_autoload_register(function ($name) {
        echo "In autoload: ";
        var_dump($name);
    });
    try {
        f();
    } catch (UndefC $u) {
        echo "In UndefClass catch block.\n";
    } catch (Exception $e) {
        echo "In Exception catch block. Autoload should not have been triggered.\n";
    }
}
fn1366733998();
--EXPECTF--
In Exception catch block. Autoload should not have been triggered.
