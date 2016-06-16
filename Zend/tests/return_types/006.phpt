--TEST--
Return type allowed in child when parent does not have return type

--FILE--
<?php

class Comment
{
}
class CommentsIterator extends ArrayIterator implements Iterator
{
    function current() : Comment
    {
        return parent::current();
    }
}
function fn133769025()
{
    $comments = new CommentsIterator([new Comment()]);
    foreach ($comments as $comment) {
        var_dump($comment);
    }
}
fn133769025();
--EXPECTF--
object(Comment)#%d (%d) {
}
