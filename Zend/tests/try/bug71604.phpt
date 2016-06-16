--TEST--
Bug #71604: Aborted Generators continue after nested finally
--FILE--
<?php

function gen()
{
    try {
        try {
            yield;
        } finally {
            print "INNER\n";
        }
    } catch (Exception $e) {
        print "EX\n";
    } finally {
        print "OUTER\n";
    }
    print "NOTREACHED\n";
}
function fn1874092215()
{
    gen()->current();
}
fn1874092215();
--EXPECT--
INNER
OUTER
