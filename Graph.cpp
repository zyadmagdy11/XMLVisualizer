#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct User
{
    string id;
    string name;
    vector<string> posts;
    vector<string> Followers_id;
};
// Weighted, Directed Graph
class Graph
{
private:
    int numVer;
    int maxVer;
    User *vertices;
    int **edges;
    bool *marks;

public:
    Graph(int max)
    {
        numVer = 0;
        maxVer = max;
        vertices = new User[max];
        edges = new int *[max];
        for (int i = 0; i < max; i++)
        {
            edges[i] = new int[max];
            for (int j = 0; j < max; j++)
            {
                edges[i][j] = 0;
            }
        }
        marks = new bool[max];
        for (int i = 0; i < max; i++)
            marks[i] = false;
    }

    ~Graph()
    {
        delete[] vertices;
        for (int i = 0; i < maxVer; i++)
        {
            delete[] edges[i];
        }
        delete[] edges;
        delete[] marks;
    }

    void AddVertex(User vertex)
    {
        if (numVer < maxVer)
        {
            vertices[numVer] = vertex;
            numVer++;
        }
        else
        {
            cout << "Max vertices reached, cannot add more users." << endl;
        }
    }

    void AddEdge(User fromVertex, User toVertex, int weight)
    {
        int row = indexOf(fromVertex);
        int col = indexOf(toVertex);
        if (row != -1 && col != -1)
        {
            edges[row][col] = weight;
        }
        else
        {
            cout << "Invalid user IDs for edge." << endl;
        }
    }
    void clearMarks()
    {
        for (int i = 0; i < numVer; i++)
            marks[i] = false;
    }

    int indexOf(User vertex)
    {
        for (int i = 0; i < numVer; i++)
        {
            if (vertices[i].id == vertex.id)
                return i; // Compare by ID
        }
        return -1;
    }
    int indexOf(string id)
    {
        for (int i = 0; i < numVer; i++)
        {
            if (vertices[i].id == id)
                return i; // Compare by ID
        }
        return -1;
    }

    void parseXML(string filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Failed to open file.\n";
            return;
        }

        string line;
        string currentId, currentName;
        vector<string> currentPosts;
        vector<string> currentFollowers;
        bool readingPosts = false, readingFollowers = false, readingID = false; // flags to indicate start of nested tag

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            if (line.find("<user>") != string::npos)
            {
                currentPosts.clear();
                currentFollowers.clear();
                readingID = true;
            }

            if (readingID && line.find("<id>") != string::npos)
            {
                currentId = line.substr(line.find("<id>") + 4, line.find("</id>") - line.find("<id>") - 4);
                readingID = false;
            }
            if (line.find("</id>") != string::npos)
            {
                readingID = false;
            }

            if (line.find("<name>") != string::npos)
            {
                currentName = line.substr(line.find("<name>") + 6, line.find("</name>") - line.find("<name>") - 6);
            }

            if (line.find("<posts>") != string::npos)
            {
                readingPosts = true;
            }

            if (readingPosts && line.find("<post>") != string::npos)
            {
                string post = line.substr(line.find("<post>") + 6, line.find("</post>") - line.find("<post>") - 6);
                currentPosts.push_back(post);
            }

            if (line.find("</posts>") != string::npos)
            {
                readingPosts = false;
            }

            if (line.find("<followers>") != string::npos)
            {
                readingFollowers = true;
            }

            if (readingFollowers && line.find("<id>") != string::npos)
            {
                string followerId = line.substr(line.find("<id>") + 4, line.find("</id>") - line.find("<id>") - 4);
                currentFollowers.push_back(followerId);
            }

            if (line.find("</followers>") != string::npos)
            {
                readingFollowers = false;
            }

            if (line.find("</user>") != string::npos)
            {
                User newUser = {currentId, currentName, currentPosts, currentFollowers};
                AddVertex(newUser);
            }
        }
        file.close();
        addEdgesBetweenUsers();
    }

    void addEdgesBetweenUsers()
    {
        for (int i = 0; i < numVer; i++)
        {
            User currentUser = vertices[i];
            for (string followerId : currentUser.Followers_id)
            {
                int followerIndex = indexOf(followerId);
                if (followerIndex != -1)
                {
                    User newFollower = vertices[followerIndex];
                    AddEdge(newFollower, currentUser, 1);
                }
                else
                {
                    // Create Unknown User if follower is not found
                    User newFollower = {followerId, "Unknown User", vector<string>(), vector<string>()};
                    AddVertex(newFollower);
                    AddEdge(newFollower, currentUser, 1);
                }
            }
        }
    }

    // Mostafa Task
    User most_influencer()
    {
        int maxFollowers = -1;
        User mostInfluentialUser;

        // Traverse each user and count the followers
        for (int i = 0; i < numVer; i++)
        {
            int followerCount = vertices[i].Followers_id.size();
            if (followerCount > maxFollowers)
            {
                maxFollowers = followerCount;
                mostInfluentialUser = vertices[i];
            }
        }

        return mostInfluentialUser;
    }
    // Ashraf Task
    User most_active()
    {
        int maxActivity = -1;
        User mostActiveUser;

        for (int i = 0; i < numVer; i++)
        {
            int activityScore = 0;

            for (int j = 0; j < numVer; j++)
            {
                if (edges[i][j] != 0)
                {
                    activityScore++;
                }
            }

            if (activityScore > maxActivity)
            {
                maxActivity = activityScore;
                mostActiveUser = vertices[i];
            }
        }

        return mostActiveUser;
    }
    // Nasser Task
    // BigO(n^2*k1*k2*numVer)
    // k1     number of Followers in temp1
    // k2     number of Followers in temp2
    // numVer number of vertices
    vector<User> findMutualFollowers(vector<string> users_id)
    {
        if (users_id.size() < 2)
            return vector<User>();

        vector<User> tempMutualFollowers;
        clearMarks();

        for (int i = 0; i < users_id.size(); ++i)
        {
            int index1 = indexOf(users_id[i]);
            if (index1 == -1)
                continue;

            User temp1 = vertices[index1];

            for (int j = 0; j < users_id.size(); ++j)
            {
                int index2 = indexOf(users_id[j]);
                if (index2 == -1 || users_id[i] == users_id[j])
                    continue;

                User temp2 = vertices[index2];

                for (string follower1 : temp1.Followers_id)
                {
                    for (string follower2 : temp2.Followers_id)
                    {
                        if (follower1 == follower2)
                        {
                            int mutualIndex = indexOf(follower1);
                            if (mutualIndex != -1 && !marks[mutualIndex])
                            {
                                marks[mutualIndex] = true;
                                tempMutualFollowers.push_back(vertices[mutualIndex]);
                            }
                        }
                    }
                }
            }
        }
        return tempMutualFollowers;
    }

    // Alfonse Task
    vector<User> suggestFollowers(string user_id)
    {
        vector<User> suggestedUsers;
        clearMarks();

        int userIndex = indexOf(user_id);
        if (userIndex == -1)
            return suggestedUsers;

        User user = vertices[userIndex];

        for (string followerId1 : user.Followers_id)
        {
            int followerIndex = indexOf(followerId1);

            if (followerIndex != -1)
            {
                User followerUser = vertices[followerIndex];

                for (string followerId2 : followerUser.Followers_id)
                {
                    int suggestedUserIndex = indexOf(followerId2);

                    if (suggestedUserIndex != -1)
                    {
                        User suggestedUser = vertices[suggestedUserIndex];

                        if (suggestedUser.id != user.id &&
                            !marks[suggestedUserIndex] &&
                            find(user.Followers_id.begin(), user.Followers_id.end(), suggestedUser.id) == user.Followers_id.end())
                        {
                            suggestedUsers.push_back(suggestedUser);
                            marks[suggestedUserIndex] = true;
                        }
                    }
                }
            }
        }
        return suggestedUsers;
    }

    // Mostafa Task
    vector<string> searchPosts(string searchTerm)
    {
        vector<string> matchedPosts;

        for (int i = 0; i < numVer; i++)
        {
            for (string post : vertices[i].posts)
            {
                if (post.find(searchTerm) != string::npos)
                {
                    matchedPosts.push_back("User: " + vertices[i].name + " (ID: " + vertices[i].id + ") - " + post);
                }
            }
        }

        return matchedPosts;
    }

    void display()
    {
        for (int i = 0; i < numVer; i++)
        {
            cout << "User Name: " << vertices[i].name << endl;
            cout << "User ID  : " << vertices[i].id << endl;
            cout << "Posts: \n";
            for (string post : vertices[i].posts)
            {
                cout << post << "\n";
            }

            cout << "\nFollowers: ";
            for (int j = 0; j < numVer; j++)
            {
                if (edges[j][i] != 0)
                {
                    cout << vertices[j].name << ", ";
                }
            }
            cout << "\n======================================\n";
        }
    }
    // Ashraf Task
    string sanitizeString(const string &input)
    {
        string sanitized = input;
        sanitized.erase(remove(sanitized.begin(), sanitized.end(), '\n'), sanitized.end());
        return sanitized;
    }
    void exportToDot(const string &filename)
    {
        ofstream outFile(filename);
        if (!outFile)
        {
            cout << "Error opening file for writing." << endl;
            return;
        }

        outFile << "digraph SocialNetwork {";

        // Output nodes (users)
        for (int i = 0; i < numVer; i++)
        {
            outFile << "  \"" << sanitizeString(vertices[i].name) << "\" [label=\"" << sanitizeString(vertices[i].name) << "\"];";
        }

        // Output edges (followers relationships)
        for (int i = 0; i < numVer; i++)
        {
            for (int j = 0; j < numVer; j++)
            {
                if (edges[i][j] != 0)
                {
                    outFile << "  \"" << sanitizeString(vertices[i].name) << "\" -> \"" << sanitizeString(vertices[j].name) << "\";";
                }
            }
        }

        outFile << "}";
        outFile.close();
    }
};
