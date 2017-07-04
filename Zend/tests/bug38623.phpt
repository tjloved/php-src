--TEST--
Bug #38623 (leaks in a tricky code with switch() and exceptions)
--FILE--
<?php

function fn1110790997()
{
    try {
        switch (strtolower("apache")) {
            case "apache":
                throw new Exception("test");
                break;
        }
    } catch (Exception $e) {
        echo "ok\n";
    }
}
fn1110790997();
--EXPECT--
ok
