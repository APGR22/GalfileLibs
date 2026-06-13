#pragma once

#include <string>
#include <list>
#include <cstdint>
#include "../../helpers/string.hpp"

namespace galfile::filesystem::path
{
    class Path
    {
        private:
            std::list<std::string> paths;

        protected:
            void _normalize_sep(std::string &str_path) const
            {
                uint64_t pos = 0;
                while ((pos = str_path.find('\\')) != std::string::npos)
                {
                    str_path[pos] = '/';
                }
            }

            void _str_path_to_list_path(
                const std::string &str_path,
                std::list<std::string> &list_path
            ) const
            {
                helper::split_string_by(list_path, str_path, '/');

                if (str_path[0] == '/')
                {
                    list_path.push_front("/");
                }
                else
                {
                    list_path.push_front(".");
                }
            }

            void _resolve_levels(std::list<std::string> &path) const
            {
                std::list<std::string> result;

                for (const std::string &pathname : path)
                {
                    if (pathname == ".." && !result.empty())
                    {
                        result.pop_back();
                    }
                    else if (pathname.starts_with('.'))
                    {
                        continue;
                    }
                    else
                    {
                        result.push_back(pathname);
                    }
                }

                if (result.empty())
                {
                    result.push_back(".");
                }
                else if (result.front() != "." && result.front() != "/")
                {
                    result.push_front(".");
                }

                path = result;
            }

            // void _filter_pathnames(std::list<std::string> &path) const
            // {
                // for (std::string &pathname : path)
                // {

                // }
            // }

            void _init(std::string str_path)
            {
                if (str_path.empty())
                {
                    this->paths.push_back(".");
                    return;
                }

                this->_normalize_sep(str_path);
                this->_str_path_to_list_path(str_path, this->paths);
                this->_resolve_levels(this->paths);
                // this->_filter_pathnames(this->paths);
            }

        public:
            Path()
            {
                this->paths.push_back(".");
            }

            Path(const std::string &str_path)
            {
                this->_init(str_path);
            }

            Path(const char *c_str_path)
            {
                std::string str_path = c_str_path;
                this->_init(str_path);
            }

            void to_parent()
            {
                if (this->paths.back() == "/") return;

                this->paths.pop_back();

                if (this->paths.empty())
                {
                    this->paths.push_back("/");
                }
            }

            Path parent() const
            {
                auto paths = this->paths;

                if (this->paths.back() != "/")
                {
                    paths.pop_back();

                    if (paths.empty())
                    {
                        paths.push_back("/");
                    }
                }

                Path path_object;
                path_object.paths = paths;

                return path_object;
            }

            void to_join_path(const Path &path)
            {
                if (path.paths.front() == "/")
                {
                    this->paths.clear();
                }

                this->paths.insert(
                    this->paths.end(),
                    ++path.paths.begin(),
                    path.paths.end()
                );

                this->_resolve_levels(this->paths);
            }

            Path join_path(const Path &path) const
            {
                auto paths = this->paths;

                if (path.paths.front() == "/")
                {
                    paths.clear();
                }

                paths.insert(paths.end(), path.paths.begin(), path.paths.end());

                this->_resolve_levels(paths);

                Path path_object;
                path_object.paths = paths;

                return path_object;
            }

            const auto &parts() const
            {
                return this->paths;
            }

            bool is_absolute() const
            {
                return this->paths.front() == "/";
            }

            Path &operator/=(const Path &path)
            {
                this->to_join_path(path);

                return *this;
            }

            Path operator/(const Path &path) const
            {
                return this->join_path(path);
            }
    };
}