
class Solution:
    def wordBreak(self, s, wordDict):
        important = []
        for word in wordDict:
            if s.find(word) == -1:
                continue
            important.append(word)
        dic = important

        memo = {}
        def backtrack_memoization(start):
            if start == len(s):
                memo[start] = True
                return True
            if start in memo:
                return memo[start]

            for word in dic:
                if s[start:start+len(word)] != word:
                    continue
                if backtrack_memoization(start + len(word)):
                    memo[start] = True
                    return True
            memo[start] = False
            return False
        return backtrack_memoization(0)

s = Solution()
# s.wordBreak("catsandog",["cats","dog","sand","and","cat"])
res = s.wordBreak("catsandog",["cats","dog","sand","and","cat"])
print(res)