--TEST--
ZE2 iterators cannot implement Traversable alone
--FILE--
<?php

class test implements Traversable
{
}
function fn111231850()
{
    $obj = new test();
    foreach ($obj as $v) {
    }
    print "Done\n";
    /* the error doesn't show the filename but 'Unknown' */
}
fn111231850();
--EXPECTF--
Fatal error: Class test must implement interface Traversable as part of either Iterator or IteratorAggregate in %s on line %d
