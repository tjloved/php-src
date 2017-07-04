--TEST--
Bug #34065 (throw in foreach causes memory leaks)
--FILE--
<?php

function fn42469014()
{
    $data = file(__FILE__);
    try {
        foreach ($data as $line) {
            throw new Exception("error");
        }
    } catch (Exception $e) {
        echo "ok\n";
    }
}
fn42469014();
--EXPECT--
ok
