--TEST--
Try catch finally (basic test)
--FILE--
<?php

function foo($throw = FALSE)
{
    try {
        echo "try\n";
        if ($throw) {
            throw new Exception("ex");
        }
    } catch (Exception $e) {
        echo "catch\n";
    } finally {
        echo "finally\n";
    }
    echo "end\n";
}
function fn1234643327()
{
    foo();
    echo "\n";
    foo(true);
}
fn1234643327();
--EXPECTF--
try
finally
end

try
catch
finally
end
