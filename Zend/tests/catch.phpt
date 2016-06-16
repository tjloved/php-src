--TEST--
catch shouldn't call __autoload
--FILE--
<?php

function __autoload($name)
{
    echo "AUTOLOAD '{$name}'\n";
    eval("class {$name} {}");
}
function fn1390939833()
{
    try {
    } catch (A $e) {
    }
    try {
        throw new Exception();
    } catch (B $e) {
    } catch (Exception $e) {
        echo "ok\n";
    }
}
fn1390939833();
--EXPECT--
ok
